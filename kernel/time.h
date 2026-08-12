#ifndef TIME_H
#define TIME_H

#include "types.h"

struct timeval {
  long tv_sec;
  long tv_usec;
};

struct tm {
  int tm_sec;   // 0-60
  int tm_min;   // 0-59
  int tm_hour;  // 0-23
  int tm_mday;  // 1-31
  int tm_mon;   // 0-11
  int tm_year;  // since 1900
  int tm_wday;  // 0-6
  int tm_yday;  // 0-365
  int tm_isdst; // zero
};

extern time_t time(time_t *);
extern int gettimeofday(struct timeval *, void *);
extern time_t mktime(struct tm *);
extern struct tm *localtime_r(const time_t *, struct tm *);
extern size_t strftime(char *, size_t, const char *, const struct tm *);

#endif
