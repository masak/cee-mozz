#ifndef I64_VALUE_H
#define I64_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    i64 payload;
} I64Value;

Offset i64_new(Arena *a, i64 payload);
I64Value *i64_resolve(Arena *a, Offset offset);
bool i64_validate(Arena *a, Offset offset, SeenSet *seenset);

Offset i64_add(Arena *a, Offset m, Offset n);
Offset i64_subtract(Arena *a, Offset m, Offset n);
Offset i64_multiply(Arena *a, Offset m, Offset n);
Offset i64_divide(Arena *a, Offset m, Offset n, Offset fallback);
Offset i64_modulo(Arena *a, Offset m, Offset n, Offset fallback);

#endif

