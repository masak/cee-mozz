#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/seenset.h"

void seenset_init(SeenSet *seenset) {
    if (seenset == NULL) {
        vm_crash(CRASH_NULL);
    }
    memset(seenset->bits, 0, sizeof(seenset->bits));
}

bool seen(SeenSet *seenset, Offset offset) {
    if (offset >= ARENA_SIZE) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }
    return (seenset->bits[offset / 8] >> (offset % 8)) & 1;
}

void seenset_add(SeenSet *seenset, Offset offset) {
    if (offset >= ARENA_SIZE) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }
    seenset->bits[offset / 8] |= (u8)(1 << (offset % 8));
}

