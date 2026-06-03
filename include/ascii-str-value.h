#ifndef ASCII_STR_VALUE_H
#define ASCII_STR_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "str.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    u64 length_in_bytes;
    char payload[];
} AsciiStrValue;

size_t ascii_str_new(Arena *a, Str *str);
AsciiStrValue *ascii_str_resolve(Arena *a, size_t offset);
bool ascii_str_validate(Arena *a, size_t offset);

size_t ascii_str_concat(Arena *a, size_t offset1, size_t offset2);
size_t generic_to_str(Arena *a, size_t offset);

#endif

