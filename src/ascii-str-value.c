#include <string.h>

#include "../include/ascii-str-value.h"
#include "../include/typedefs.h"
#include "../include/types.h"

size_t ascii_str_new(Arena *a, Str *str) {
    u64 length_in_bytes = str->length_in_bytes;
    AsciiStrValue *ascii_str_value = arena_alloc(
        a,
        sizeof(AsciiStrValue) + length_in_bytes,
        alignof(AsciiStrValue)
    );
    ascii_str_value->tag = TAG_ASCII_STR;
    ascii_str_value->length_in_bytes = length_in_bytes;
    memcpy(ascii_str_value->payload, str->payload, length_in_bytes);
    return (size_t)((unsigned char *)ascii_str_value - a->bytes);
}

/* Resolve an offset back to a pointer. */
AsciiStrValue *ascii_str_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(AsciiStrValue));
    return (AsciiStrValue *)(a->bytes + offset);
}

