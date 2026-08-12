//
// formatted console output -- printk, panic.
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

volatile int panicking = 0; // printing a panic message
volatile int panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static struct {
  struct spinlock lock;
} pr;

static char digits[] = "0123456789abcdef";

static void
printint(void (*putch)(int, void *), void *arg, long long xx, int base,
         int sign, int width, int padc)
{
  char buf[20];
  int i;
  unsigned long long x;

  if (sign && (sign = (xx < 0)))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);

  if (sign && padc == '0')
    putch('-', arg);
  for (int w = i + sign; w < width; w++)
    putch(padc, arg);
  if (sign && padc != '0')
    putch('-', arg);

  while (--i >= 0)
    putch(buf[i], arg);
}

static void
printptr(void (*putch)(int, void *), void *arg, uint64 x)
{
  int i;
  putch('0', arg);
  putch('x', arg);
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putch(digits[x >> (sizeof(uint64) * 8 - 4)], arg);
}

static void
kvprintf(void (*putch)(int, void *), void *arg, const char *fmt, va_list ap)
{
  int i, cx, c0, width, padc, lflag;
  char *s;

  for (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    if (cx != '%') {
      putch(cx, arg);
      continue;
    }
    i++;
    padc = ' ';
    if ((fmt[i] & 0xff) == '0')
      padc = '0';
    width = 0;
    for (; (c0 = fmt[i] & 0xff) >= '0' && c0 <= '9'; i++)
      width = width * 10 + c0 - '0';
    lflag = 0;
    for (; (c0 = fmt[i] & 0xff) == 'l'; i++)
      lflag++;
    if (c0 == 'z') { // size_t is 64-bit
      lflag = 1;
      i++;
      c0 = fmt[i] & 0xff;
    }
    if (c0 == 'd') {
      if (lflag)
        printint(putch, arg, va_arg(ap, int64_t), 10, 1, width, padc);
      else
        printint(putch, arg, va_arg(ap, int), 10, 1, width, padc);
    } else if (c0 == 'u') {
      if (lflag)
        printint(putch, arg, va_arg(ap, uint64), 10, 0, width, padc);
      else
        printint(putch, arg, va_arg(ap, uint32), 10, 0, width, padc);
    } else if (c0 == 'x') {
      if (lflag)
        printint(putch, arg, va_arg(ap, uint64), 16, 0, width, padc);
      else
        printint(putch, arg, va_arg(ap, uint32), 16, 0, width, padc);
    } else if (c0 == 'p') {
      printptr(putch, arg, va_arg(ap, uint64));
    } else if (c0 == 'c') {
      putch(va_arg(ap, uint), arg);
    } else if (c0 == 's') {
      if ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      for (; *s; s++)
        putch(*s, arg);
    } else if (c0 == '%') {
      putch('%', arg);
    } else if (c0 == 0) {
      break;
    } else {
      // Print unknown % sequence to draw attention.
      putch('%', arg);
      putch(c0, arg);
    }
  }
}

static void
cons_putch(int c, void *arg)
{
  consputc(c);
}

// Print to the console.
int
printk(char *fmt, ...)
{
  va_list ap;

  if (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  kvprintf(cons_putch, 0, fmt, ap);
  va_end(ap);

  if (panicking == 0)
    release(&pr.lock);

  return 0;
}

struct sprintbuf {
  char *buf;
  char *ebuf;
  int cnt;
};

static void
sprint_putch(int c, void *arg)
{
  struct sprintbuf *b = arg;

  if (b->buf < b->ebuf)
    *b->buf++ = c;
  b->cnt++;
}

int
vsnprintf(char *buf, size_t n, const char *fmt, va_list ap)
{
  struct sprintbuf b;

  if (n == 0)
    return 0;

  b = (struct sprintbuf){buf, buf + n - 1, 0};
  kvprintf(sprint_putch, &b, fmt, ap);
  *b.buf = '\0';

  return b.cnt;
}

int
snprintf(char *buf, size_t n, const char *fmt, ...)
{
  va_list ap;
  int cnt;

  va_start(ap, fmt);
  cnt = vsnprintf(buf, n, fmt, ap);
  va_end(ap);

  return cnt;
}

void
panic(char *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  for (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
