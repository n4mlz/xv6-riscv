#include <stdio.h>
#include <stdarg.h>

#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

struct _FILE {
  struct spinlock lock;
  int depth; /* recursion count (POSIX flockfile allows recursive locking) */
};

static FILE _stderr;

FILE *stderr = &_stderr;

void
flockfile(FILE *fp)
{
  if (holding(&fp->lock)) { /* already locked by this context: just recurse */
    fp->depth++;
    return;
  }
  acquire(&fp->lock);
  fp->depth = 1;
}

void
funlockfile(FILE *fp)
{
  if (--fp->depth == 0)
    release(&fp->lock);
}

int
vfprintf(FILE *fp, const char *fmt, va_list ap)
{
  char buf[256];
  int n;

  n = vsnprintf(buf, sizeof(buf), fmt, ap);
  printk("%s", buf);
  return n;
}

int
fprintf(FILE *fp, const char *fmt, ...)
{
  va_list ap;
  int n;

  va_start(ap, fmt);
  n = vfprintf(fp, fmt, ap);
  va_end(ap);
  return n;
}
