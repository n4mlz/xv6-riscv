#include "platform.h"

#include "util.h"
#include "net.h"

static uint32 seed = 1;

int
platform_init(void)
{
  seed = rtcread();
  if (intr_init() == -1) {
    return -1;
  }
  intr_register(INTR_IRQ_SOFT, net_softirq_handler, 0, NULL);
  return 0;
}

int
platform_run(void)
{
  if (intr_run() == -1) {
    return -1;
  }
  return 0;
}

int
platform_shutdown(void)
{
  return 0;
}

/*
 * Memory
 */

void *
memory_alloc(size_t size)
{
  void *p;

  if (PGSIZE < size) {
    return NULL;
  }
  p = kalloc();
  if (p) {
    memset(p, 0, size);
  }
  return p;
}

void
memory_free(void *ptr)
{
  kfree(ptr);
}

/*
 * Lock
 */

int
lock_init(lock_t *lock)
{
  initlock(lock, "");
  return 0;
}

int
lock_acquire(lock_t *lock)
{
  acquire(lock);
  return 0;
}

int
lock_release(lock_t *lock)
{
  release(lock);
  return 0;
}

/*
 * Random
 */

uint16_t
random16(void)
{
  /* Linear Congruential Generator (LCG) */
  seed = seed * 1103515245 + 12345;
  return (seed >> 16) & 0xffff;
}
