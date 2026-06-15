#include <string.h>

#include "../include/arena.h"
#include "../include/seenset.h"

void seenset_init(SeenSet *seenset) {
    memset(seenset->bits, 0, sizeof(seenset->bits));
}

bool seen(SeenSet *seenset, Offset offset) {
    assert(offset < ARENA_SIZE);
    return (seenset->bits[offset / 8] >> (offset % 8)) & 1;
}

void seenset_add(SeenSet *seenset, Offset offset) {
    assert(offset < ARENA_SIZE);
    seenset->bits[offset / 8] |= (u8)(1 << (offset % 8));
}

