#ifndef STR_H
#define STR_H

#include "typedefs.h"

typedef struct {
    u64 length_in_bytes;
    char payload[];
} Str;

/* STR("hello") -> Str*
 * Works only with string literals. The implicit NUL byte is excluded
 * from `length_in_bytes`, but it is harmless if it sits one byte past it. */
#define STR(s) \
    ((Str *)&(struct { u64 length_in_bytes; char payload[sizeof(s)]; }){ \
        sizeof(s) - 1, s })

#endif

