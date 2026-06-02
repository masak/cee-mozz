#ifndef VALUE_H
#define VALUE_H

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
} Value;

u64 value_tag(Arena *a, size_t offset);

#endif

