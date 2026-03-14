/* stdio.h -- openfpgaOS libc jump table wrapper */
#ifndef _OF_STDIO_H
#define _OF_STDIO_H

#ifdef OF_PC
#include_next <stdio.h>
#else

#include "../of_libc.h"

typedef void FILE;
#define stdout (__of_libc->stdout_ptr)
#define stderr (__of_libc->stderr_ptr)
#define NULL   ((void *)0)
#define EOF    (-1)

/* Variadic functions use macros to call through the table */
#define snprintf  __of_libc->snprintf
#define vsnprintf __of_libc->vsnprintf
#define printf    __of_libc->printf
#define fprintf   __of_libc->fprintf

#endif /* OF_PC */
#endif /* _OF_STDIO_H */
