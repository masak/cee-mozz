#ifndef INT_TABLE_H
#define INT_TABLE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    u32 length;
    Offset elements[];
} IntTable;

Offset inttable_new(Arena *a, u32 length, Offset elements[]);
IntTable *inttable_resolve(Arena *a, Offset offset);
bool inttable_validate(Arena *a, Offset offset, SeenSet *seenset);

Offset inttable_get(Arena *a, Offset offset, u32 index);

#endif

