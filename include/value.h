#ifndef VALUE_H
#define VALUE_H

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
} Value;

u64 value_tag(Arena *a, Offset offset);
bool generic_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

