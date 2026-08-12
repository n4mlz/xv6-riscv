#ifndef STDLIB_H
#define STDLIB_H

#include <sys/types.h>

extern long strtol(const char *s, char **endptr, int base);

extern void srand(unsigned int newseed);
extern long random(void);

#endif
