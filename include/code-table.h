#ifndef CODE_TABLE_H
#define CODE_TABLE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    u32 length;
    Offset elements[];
} CodeTable;

Offset codetable_new(Arena *a, u32 length, Offset elements[]);
CodeTable *codetable_resolve(Arena *a, Offset offset);
bool codetable_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

