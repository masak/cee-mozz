#include "../include/arena.h"
#include "../include/value.h"

u64 value_tag(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(Value));
    return ((Value *)(a->bytes + offset))->tag;
}

