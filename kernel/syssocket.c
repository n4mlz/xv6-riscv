//
// Socket system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into net/sock.c.
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

#include "net/sock.h"

#define KBUFSIZ 2048

// Allocate a struct file and a file descriptor for the given socket
// descriptor. Closes the socket and returns -1 on failure.
static int
sockfdalloc(int sock)
{
  struct file *f;
  int fd;

  if ((f = filealloc()) == 0) {
    sock_close(sock);
    return -1;
  }
  f->type = FD_SOCKET;
  f->readable = 1;
  f->writable = 1;
  f->sock = sock;
  if ((fd = fdalloc(f)) < 0) {
    fileclose(f); // also closes the socket
    return -1;
  }
  return fd;
}

// Fetch the nth word-sized system call argument as a socket descriptor.
static int
argsock(int n, int *psock)
{
  struct file *f;

  if (argfd(n, 0, &f) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  *psock = f->sock;
  return 0;
}

// Fetch a struct sockaddr_in from user space.
static int
fetchsockaddr(uint64 addr, int addrlen, struct sockaddr_in *sin)
{
  struct proc *p = myproc();

  if (addrlen != sizeof(*sin))
    return -1;
  if (copyin(p->pagetable, (char *)sin, addr, sizeof(*sin)) < 0)
    return -1;
  return 0;
}

// Copy out a struct sockaddr_in and its length to user space.
// Does nothing if the user passed a null pointer for either of them.
static int
storesockaddr(uint64 addr, uint64 addrlen, struct sockaddr_in *sin, int len)
{
  struct proc *p = myproc();

  if (addr == 0 || addrlen == 0)
    return 0;
  if (copyout(p->pagetable, addr, (char *)sin, len) < 0)
    return -1;
  if (copyout(p->pagetable, addrlen, (char *)&len, sizeof(len)) < 0)
    return -1;
  return 0;
}

uint64
sys_socket(void)
{
  int domain, type, protocol;
  int sock;

  argint(0, &domain);
  argint(1, &type);
  argint(2, &protocol);
  if ((sock = sock_open(domain, type, protocol)) < 0)
    return -1;
  return sockfdalloc(sock);
}

uint64
sys_bind(void)
{
  int sock, addrlen;
  uint64 addr;
  struct sockaddr_in sin;

  argaddr(1, &addr);
  argint(2, &addrlen);
  if (argsock(0, &sock) < 0)
    return -1;
  if (fetchsockaddr(addr, addrlen, &sin) < 0)
    return -1;
  return sock_bind(sock, (struct sockaddr *)&sin, sizeof(sin));
}

uint64
sys_recvfrom(void)
{
  int sock, n, len;
  uint64 buf, addr, addrlen;
  char kbuf[KBUFSIZ];
  struct sockaddr_in sin;
  struct proc *p = myproc();
  ssize_t ret;

  argaddr(1, &buf);
  argint(2, &n);
  argaddr(3, &addr);
  argaddr(4, &addrlen);
  if (argsock(0, &sock) < 0)
    return -1;
  if (n < 0)
    return -1;
  if (n > KBUFSIZ)
    n = KBUFSIZ;
  len = sizeof(sin);
  if ((ret = sock_recvfrom(sock, kbuf, n, (struct sockaddr *)&sin, &len)) < 0)
    return -1;
  // copy out only the bytes actually received, not the whole buffer
  if (ret > 0 && copyout(p->pagetable, buf, kbuf, ret) < 0)
    return -1;
  if (storesockaddr(addr, addrlen, &sin, len) < 0)
    return -1;
  return ret;
}

uint64
sys_sendto(void)
{
  int sock, n, addrlen;
  uint64 buf, addr;
  char kbuf[KBUFSIZ];
  struct sockaddr_in sin;
  struct proc *p = myproc();

  argaddr(1, &buf);
  argint(2, &n);
  argaddr(3, &addr);
  argint(4, &addrlen);
  if (argsock(0, &sock) < 0)
    return -1;
  if (n < 0 || n > KBUFSIZ)
    return -1;
  if (fetchsockaddr(addr, addrlen, &sin) < 0)
    return -1;
  if (copyin(p->pagetable, kbuf, buf, n) < 0)
    return -1;
  return sock_sendto(sock, kbuf, n, (struct sockaddr *)&sin, sizeof(sin));
}
