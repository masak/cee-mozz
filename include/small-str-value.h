#ifndef SMALL_STR_VALUE_H
#define SMALL_STR_VALUE_H

#include <stdbool.h>
#include <string.h>

#include "arena.h"
#include "outcome.h"
#include "seenset.h"
#include "typedefs.h"

#define SMALL_STR_MAX_BYTES 8

typedef struct {
    Tag tag;
    u8 payload[8];
} SmallStrValue;

Offset small_str_new(Arena *a, s8 *str);
SmallStrValue *small_str_resolve(Arena *a, Offset offset);
bool small_str_validate(Arena *a, Offset offset, SeenSet *seenset);

Outcome small_str_concat(
    Arena *a,
    Offset offset1,
    Offset offset2,
    Offset *out_offset
);

#endif

