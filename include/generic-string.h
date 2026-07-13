#ifndef GENERIC_STRING_H
#define GENERIC_STRING_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

bool utf8_validate_and_count(u8 *bytes, u32 length, u32 *out_count);
bool is_generic_string(Arena *a, Offset offset);

#endif

