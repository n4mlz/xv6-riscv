/* stub: to be implemented in the software interrupt step */

#include "platform.h"

int
intr_register(unsigned int irq, intr_isr_t isr, int flags, void *arg)
{
  return 0;
}

int
intr_raise(unsigned int irq)
{
  return 0;
}

int
intr_init(void)
{
  return 0;
}

int
intr_run(void)
{
  return 0;
}

int
intr_shutdown(void)
{
  return 0;
}
