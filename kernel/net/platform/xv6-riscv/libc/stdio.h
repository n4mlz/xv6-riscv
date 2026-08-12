#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include <sys/types.h>

typedef struct _FILE FILE;

extern FILE *stderr;

extern int fprintf(FILE *fp, const char *fmt, ...);
extern int vfprintf(FILE *fp, const char *fmt, va_list ap);
extern int snprintf(char *buf, size_t n, const char *fmt, ...);
extern int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap);
extern void flockfile(FILE *fp);
extern void funlockfile(FILE *fp);

#endif
