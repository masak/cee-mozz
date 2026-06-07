#ifndef STR_TABLE_H
#define STR_TABLE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    u32 length;
    Offset elements[];
} StrTable;

Offset strtable_new(Arena *a, u32 length, Offset elements[]);
StrTable *strtable_resolve(Arena *a, Offset offset);
bool strtable_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

