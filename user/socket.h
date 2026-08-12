//
// Socket API definitions for user programs.
// Struct layouts must match the kernel's definitions in kernel/net/sock.h.
//

#define PF_UNSPEC 0
#define PF_INET   2
#define PF_INET6  10

#define AF_UNSPEC PF_UNSPEC
#define AF_INET   PF_INET
#define AF_INET6  PF_INET6

#define SOCK_STREAM 1
#define SOCK_DGRAM  2

#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

#define INADDR_ANY 0

struct in_addr {
  uint s_addr;
};

struct sockaddr {
  ushort sa_family;
  char sa_data[14];
};

struct sockaddr_in {
  ushort sin_family;
  ushort sin_port;
  struct in_addr sin_addr;
};
