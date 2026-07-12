#ifndef ASCII_STR_VALUE_H
#define ASCII_STR_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "outcome.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    u32 length_in_bytes;
    char payload[];
} AsciiStrValue;

Offset ascii_str_new(Arena *a, s8 *str);
AsciiStrValue *ascii_str_resolve(Arena *a, Offset offset);
bool ascii_str_validate(Arena *a, Offset offset, SeenSet *seenset);

Outcome ascii_str_concat(
    Arena *a,
    Offset offset1,
    Offset offset2,
    Offset *out_offset
);

#endif

