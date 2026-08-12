#include "platform.h"

#include "util.h"

int
net_init(void)
{
  char msg[] = "Hello, SecCamp2026!";

  debugf("%s", msg);
  debugdump(msg, sizeof(msg));

  return 0;
}
