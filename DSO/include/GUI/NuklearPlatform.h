#ifndef NUKLEAR_PLATFORM_H
#define NUKLEAR_PLATFORM_H

#include <string.h>
#include <stdarg.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS

#if defined(__cplusplus)
extern "C" {
#endif

double nkplat_strtod(const char *, const char **);
char *nkplat_dtoa(char *buf, double n);

#if defined(__cplusplus)
}
#endif


#define NK_MEMSET memset
#define NK_MEMCPY memcpy
#define NK_SQRT sqrtf
#define NK_SIN sinf
#define NK_COS cosf
#define NK_STRTOD nkplat_strtod
#define NK_DTOA nkplat_dtoa

#include <nuklear.h>

#endif
