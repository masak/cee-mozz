#ifndef ASCII_STR_VALUE_H
#define ASCII_STR_VALUE_H

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    u64 length_in_bytes;
    char payload[];
} AsciiStrValue;

size_t ascii_str_new(Arena *a, char *payload);
AsciiStrValue *ascii_str_resolve(Arena *a, size_t offset);

#endif

