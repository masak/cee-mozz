#ifndef I64_VALUE_H
#define I64_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "outcome.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    i64 payload;
} I64Value;

Offset i64_new(Arena *a, i64 payload);
I64Value *i64_resolve(Arena *a, Offset offset);
bool i64_validate(Arena *a, Offset offset, SeenSet *seenset);

Outcome i64_add(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome i64_subtract(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome i64_multiply(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome i64_divide(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome i64_modulo(Arena *a, Offset m, Offset n, Offset *out_offset);

#endif

