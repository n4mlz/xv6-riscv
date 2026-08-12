#ifndef STRING_H
#define STRING_H

#include "types.h"

extern int memcmp(const void *v1, const void *v2, uint n);
extern void *memmove(void *dst, const void *src, uint n);
extern void *memcpy(void *dst, const void *src, uint n);
extern void *memset(void *dst, int c, uint n);
extern int strlen(const char *s);
extern int strncmp(const char *p, const char *q, uint n);
extern char *strncpy(char *s, const char *t, int n);
extern char *strrchr(const char *cp, int ch);

#endif
