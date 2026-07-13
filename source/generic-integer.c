#include <stdbool.h>

#include "../include/arena.h"
#include "../include/tags.h"
#include "../include/typedefs.h"
#include "../include/value.h"

bool is_generic_integer(Arena *a, Offset offset) {
    Tag tag = value_tag(a, offset);
    return tag == TAG_I64 || tag == TAG_INT;
}

