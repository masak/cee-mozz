#ifndef VALUE_H
#define VALUE_H

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
} Value;

Tag value_tag(Arena *a, Offset offset);
bool generic_validate(Arena *a, Offset offset, SeenSet *seenset);

Offset generic_to_str(Arena *a, Offset offset);

#endif

