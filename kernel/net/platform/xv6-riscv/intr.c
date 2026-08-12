#include <stdio.h>

#include "platform.h"

#include "util.h"

struct irq_entry {
  struct irq_entry *next;
  unsigned int irq;
  intr_isr_t isr;
  int flags;
  void *arg;
};

/*
 * NOTE: if you want to add/delete the entries after intr_run(),
 *       you need to protect these lists with a mutex.
 */
static struct irq_entry *irqs;

int
intr_register(unsigned int irq, intr_isr_t isr, int flags, void *arg)
{
  struct irq_entry *entry;

  for (entry = irqs; entry; entry = entry->next) {
    if (entry->irq == irq) {
      if (entry->flags ^ INTR_IRQ_SHARED || flags ^ INTR_IRQ_SHARED) {
        errorf("conflicts with already registered IRQs, irq=%u", irq);
        return -1;
      }
    }
  }
  entry = memory_alloc(sizeof(*entry));
  if (!entry) {
    errorf("memory_alloc() failure");
    return -1;
  }
  entry->irq = irq;
  entry->isr = isr;
  entry->flags = flags;
  entry->arg = arg;
  entry->next = irqs;
  irqs = entry;
  infof("success, irq=%u", irq);
  return 0;
}

int
intr_raise(unsigned int irq)
{
  return 0;
}

/*
 * NOTE: called from devintr() in kernel/trap.c
 */
void
intr_dispatch(unsigned int irq)
{
  struct irq_entry *entry;

  for (entry = irqs; entry; entry = entry->next) {
    if (entry->irq == irq) {
      entry->isr(entry->irq, entry->arg);
    }
  }
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
