#ifndef ASCII_STR_VALUE_H
#define ASCII_STR_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    u64 length_in_bytes;
    char payload[];
} AsciiStrValue;

Offset ascii_str_new(Arena *a, s8 *str);
AsciiStrValue *ascii_str_resolve(Arena *a, Offset offset);
bool ascii_str_validate(Arena *a, Offset offset);

Offset ascii_str_concat(Arena *a, Offset offset1, Offset offset2);
Offset generic_to_str(Arena *a, Offset offset);

#endif

