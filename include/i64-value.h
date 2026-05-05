#ifndef I64_VALUE_H
#define I64_VALUE_H

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    i64 payload;
} I64Value;

size_t i64_new(Arena *a, i64 payload);
I64Value *i64_resolve(Arena *a, size_t offset);
size_t i64_add(Arena *a, size_t m, size_t n);
size_t i64_subtract(Arena *a, size_t m, size_t n);

#endif

