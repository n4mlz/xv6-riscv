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

static lock_t pendinglock = LOCK_INITIALIZER;
static uint64_t pending;

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

/*
 * NOTE: only accepts soft IRQs (single bit value out of PLIC source range)
 */
int
intr_raise(unsigned int irq)
{
  lock_acquire(&pendinglock);
  pending |= irq;
  lock_release(&pendinglock);
  w_sip(r_sip() | SIP_SSIP);
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

/*
 * NOTE: called from devintr() in kernel/trap.c
 */
void
intr_soft_dispatch(void)
{
  uint64_t irqs, irq;

  // clear SSIP before taking the pending snapshot, so that an irq
  // raised while the ISRs run re-triggers the software interrupt.
  w_sip(r_sip() & ~SIP_SSIP);

  lock_acquire(&pendinglock);
  irqs = pending;
  pending = 0;
  lock_release(&pendinglock);

  for (irq = 1; irqs; irq <<= 1) {
    if (irqs & irq) {
      intr_dispatch(irq);
      irqs &= ~irq;
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
