#ifndef INT_VALUE_H
#define INT_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "outcome.h"
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

Outcome int_add(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome int_subtract(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome int_multiply(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome int_divide(Arena *a, Offset m, Offset n, Offset *out_offset);
Outcome int_modulo(Arena *a, Offset m, Offset n, Offset *out_offset);

Offset int_to_str(Arena *a, Offset offset);

#endif

