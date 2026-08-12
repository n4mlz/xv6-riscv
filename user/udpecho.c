//
// UDP echo server.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/socket.h"
#include "user/user.h"

#define ECHO_PORT 7
#define BUFSIZ    1024

int
main(int argc, char *argv[])
{
  int soc;
  struct sockaddr_in self, peer;
  int peerlen;
  char buf[BUFSIZ];
  int ret;
  uchar *addr;

  soc = socket(AF_INET, SOCK_DGRAM, 0);
  if (soc == -1) {
    printf("socket: failure\n");
    exit(1);
  }
  memset(&self, 0, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_addr.s_addr = INADDR_ANY;
  self.sin_port = htons(ECHO_PORT);
  if (bind(soc, (struct sockaddr *)&self, sizeof(self)) == -1) {
    printf("bind: failure\n");
    close(soc);
    exit(1);
  }
  printf("waiting for message on port %d...\n", ECHO_PORT);
  while (1) {
    peerlen = sizeof(peer);
    ret = recvfrom(soc, buf, sizeof(buf), (struct sockaddr *)&peer, &peerlen);
    if (ret == -1) {
      printf("recvfrom: failure\n");
      break;
    }
    addr = (uchar *)&peer.sin_addr.s_addr;
    printf("%d bytes received from %d.%d.%d.%d:%d\n", ret, addr[0], addr[1],
           addr[2], addr[3], ntohs(peer.sin_port));
    if (sendto(soc, buf, ret, (struct sockaddr *)&peer, sizeof(peer)) == -1) {
      printf("sendto: failure\n");
      break;
    }
  }
  close(soc);
  exit(0);
}
