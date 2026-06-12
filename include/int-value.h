#ifndef INT_VALUE_H
#define INT_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    u32 sign;
    u32 length;
    u32 payload[];
} IntValue;

/* General constructor from raw limbs. */
Offset int_new(Arena *a, u32 sign, u32 length, u32 payload[]);
IntValue *int_resolve(Arena *a, Offset offset);
bool int_validate(Arena *a, Offset offset, SeenSet *seenset);

Offset int_add(Arena *a, Offset m, Offset n);
Offset int_subtract(Arena *a, Offset m, Offset n);
Offset int_multiply(Arena *a, Offset m, Offset n);
Offset int_divide(Arena *a, Offset m, Offset n, Offset fallback);
Offset int_modulo(Arena *a, Offset m, Offset n, Offset fallback);

Offset int_to_str(Arena *a, Offset offset);

#endif

