#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <inttypes.h>

typedef uint8_t     u8;

typedef int32_t     i32;

typedef uint64_t    u64;
typedef int64_t     i64;

typedef ptrdiff_t   size;

#define countof(a)      (size)(sizeof(a) / sizeof(*(a)))
#define lengthof(s)     (countof(s) - 1)

#define s8(s) (s8){(u8 *)s, lengthof(s)}
typedef struct {
    u8 *payload;
    size length_in_bytes;
} s8;

#endif

