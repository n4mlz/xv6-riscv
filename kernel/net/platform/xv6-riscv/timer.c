#include <stdio.h>
#include <sys/time.h>

#include "platform.h"

#include "util.h"

int
timer_register(struct timeval interval, void (*handler)(void))
{
  return 0;
}

int
timer_init(void)
{
  return 0;
}

int
timer_run(void)
{
  return 0;
}

int
timer_shutdown(void)
{
  return 0;
}
