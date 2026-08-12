#include <stdio.h>
#include <errno.h>
#include <time.h>

#include "platform.h"

#include "util.h"

int
sched_task_init(struct sched_task *task)
{
  task->next = NULL;
  task->interrupted = 0;
  task->wc = 0;
  return 0;
}

int
sched_task_destroy(struct sched_task *task)
{
  if (task->wc) {
    return -1;
  }
  return 0;
}

/*
 * NOTE: sleep() releases the lock while sleeping and reacquires it on wakeup,
 *       so the caller can use it just like pthread_cond_wait().
 */
int
sched_task_sleep(struct sched_task *task, lock_t *lock,
                 const struct timespec *abstime)
{
  (void)abstime; /* timeout is not supported */

  if (task->interrupted) {
    errno = EINTR;
    return -1;
  }
  task->wc++;
  sleep(task, lock);
  task->wc--;
  if (task->interrupted) {
    if (!task->wc) {
      task->interrupted = 0;
    }
    errno = EINTR;
    return -1;
  }
  return 0;
}

int
sched_task_wakeup(struct sched_task *task)
{
  wakeup(task);
  return 0;
}

int
sched_init(void)
{
  return 0;
}

int
sched_run(void)
{
  return 0;
}

int
sched_shutdown(void)
{
  return 0;
}
