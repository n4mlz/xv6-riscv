#ifndef SYS_TIME_H
#define SYS_TIME_H

#include <time.h>

#define timerclear(tv) ((tv)->tv_sec = (tv)->tv_usec = 0)

#define timersub(a, b, res)                                                    \
  do {                                                                         \
    (res)->tv_sec = (a)->tv_sec - (b)->tv_sec;                                 \
    (res)->tv_usec = (a)->tv_usec - (b)->tv_usec;                              \
    if ((res)->tv_usec < 0) {                                                  \
      (res)->tv_sec--;                                                         \
      (res)->tv_usec = 1000000;                                                \
    }                                                                          \
  } while (0)

#define timercmp(a, b, CMP)                                                    \
  (((a)->tv_sec == (b)->tv_sec) ? ((a)->tv_usec CMP(b)->tv_usec)               \
                                : ((a)->tv_sec CMP(b)->tv_sec))

#endif
