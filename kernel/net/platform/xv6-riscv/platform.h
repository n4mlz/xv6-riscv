#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include <sys/types.h>
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

extern int platform_init(void);
extern int platform_run(void);
extern int platform_shutdown(void);

/*
 * Memory
 */

extern void *memory_alloc(size_t size);
extern void memory_free(void *ptr);

/*
 * Lock
 */

typedef struct spinlock lock_t;

#define LOCK_INITIALIZER {0}

extern int lock_init(lock_t *lock);
extern int lock_acquire(lock_t *lock);
extern int lock_release(lock_t *lock);

/*
 * Random
 */

extern uint16_t random16(void);

#include "intr.h"

#endif
