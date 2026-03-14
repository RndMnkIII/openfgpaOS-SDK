/*
 * of_libc.h -- openfpgaOS C Library Jump Table
 *
 * The OS populates a table of libc function pointers at a fixed address
 * before launching the application. Apps call standard C functions
 * through this table with zero syscall overhead (indirect call only).
 *
 * The table is append-only and versioned: new functions are added at
 * the end, old apps on new OS work unchanged, and apps can check
 * `count` for compatibility with newer functions.
 *
 * Apps should NOT include this header directly. Instead, use standard
 * headers (<math.h>, <string.h>, etc.) which resolve through the table
 * when compiled with -I<path-to-libc_include>.
 */

#ifndef OF_LIBC_H
#define OF_LIBC_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define OF_LIBC_MAGIC   0x4F46434C  /* 'OFCL' */
#define OF_LIBC_VERSION 1
#define OF_LIBC_ADDR    0x103FF000

struct of_libc_table {
    uint32_t magic;
    uint32_t version;
    uint32_t count;         /* number of entries after this header */
    uint32_t _reserved;

    /* -- memory (4) -- */
    void *(*malloc)(size_t);
    void (*free)(void *);
    void *(*realloc)(void *, size_t);
    void *(*calloc)(size_t, size_t);

    /* -- string (12) -- */
    void *(*memcpy)(void *, const void *, size_t);
    void *(*memset)(void *, int, size_t);
    void *(*memmove)(void *, const void *, size_t);
    int (*memcmp)(const void *, const void *, size_t);
    size_t (*strlen)(const char *);
    int (*strcmp)(const char *, const char *);
    int (*strncmp)(const char *, const char *, size_t);
    char *(*strcpy)(char *, const char *);
    char *(*strncpy)(char *, const char *, size_t);
    char *(*strstr)(const char *, const char *);
    char *(*strchr)(const char *, int);
    char *(*strrchr)(const char *, int);

    /* -- stdio (6) -- */
    int (*snprintf)(char *, size_t, const char *, ...);
    int (*vsnprintf)(char *, size_t, const char *, va_list);
    int (*printf)(const char *, ...);
    int (*fprintf)(void *, const char *, ...);
    void *stdout_ptr;       /* musl FILE* for stdout */
    void *stderr_ptr;       /* musl FILE* for stderr */

    /* -- stdlib (9) -- */
    int (*abs)(int);
    long (*labs)(long);
    int (*atoi)(const char *);
    long (*strtol)(const char *, char **, int);
    unsigned long (*strtoul)(const char *, char **, int);
    void (*qsort)(void *, size_t, size_t, int(*)(const void *, const void *));
    void *(*bsearch)(const void *, const void *, size_t, size_t,
                     int (*)(const void *, const void *));
    int (*rand)(void);
    void (*srand)(unsigned int);

    /* -- math (21) -- */
    float (*sinf)(float);
    float (*cosf)(float);
    float (*tanf)(float);
    float (*asinf)(float);
    float (*acosf)(float);
    float (*atan2f)(float, float);
    float (*sqrtf)(float);
    float (*fmodf)(float, float);
    float (*floorf)(float);
    float (*ceilf)(float);
    float (*roundf)(float);
    float (*fabsf)(float);
    float (*fmaxf)(float, float);
    float (*fminf)(float, float);
    float (*powf)(float, float);
    float (*logf)(float);
    float (*log2f)(float);
    float (*expf)(float);
    double (*sin)(double);
    double (*cos)(double);
    double (*sqrt)(double);
};

/* Total function pointers + data pointers in the table (excluding header) */
#define OF_LIBC_COUNT 52

/* App-side accessor */
#define __of_libc ((const struct of_libc_table *)OF_LIBC_ADDR)

#endif /* OF_LIBC_H */
