#ifndef SEENSET_H
#define SEENSET_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u8 bits[(ARENA_SIZE + 7) / 8];
} SeenSet;

void seenset_init(SeenSet *seenset);
bool seen(SeenSet *seenset, Offset offset);
void seenset_add(SeenSet *seenset, Offset offset);

#endif

