#ifndef STR_VALUE_H
#define STR_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "outcome.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    u32 length_in_bytes;
    u32 length_in_codepoints;
    u8 payload[];
} StrValue;

Offset str_new(Arena *a, s8 *str);
StrValue *str_resolve(Arena *a, Offset offset);
bool str_validate(Arena *a, Offset offset, SeenSet *seenset);

Outcome str_concat(
    Arena *a,
    Offset offset1,
    Offset offset2,
    Offset *out_offset
);

#endif

