#include <stdio.h>
#include <string.h>

#include "../include/ascii-str-value.h"
#include "../include/i64-value.h"
#include "../include/typedefs.h"
#include "../include/types.h"
#include "../include/value.h"

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

size_t ascii_str_concat(Arena *a, size_t offset1, size_t offset2) {
    AsciiStrValue *lhs = ascii_str_resolve(a, offset1);
    AsciiStrValue *rhs = ascii_str_resolve(a, offset2);

    u64 length_in_bytes = lhs->length_in_bytes + rhs->length_in_bytes;
    u64 total_size = sizeof(AsciiStrValue) + length_in_bytes;

    AsciiStrValue *result = arena_alloc(a, total_size, alignof(AsciiStrValue));
    result->tag = TAG_ASCII_STR;
    result->length_in_bytes = length_in_bytes;

    memcpy(result->payload, lhs->payload, lhs->length_in_bytes);
    memcpy(
        result->payload + lhs->length_in_bytes,
        rhs->payload,
        rhs->length_in_bytes
    );

    return (size_t)((unsigned char *)result - a->bytes);
}

/* Convert any supported value to its AsciiStrValue representation */
size_t generic_to_str(Arena *a, size_t offset) {
    switch (value_tag(a, offset)) {
        case TAG_I64: {
            I64Value *value = i64_resolve(a, offset);

            /* i64 range: -9223372036854775808 .. 9223372036854775807
             * maximum string length is 20 characters (19 digits + sign). */
            char buffer[32];
            int n = snprintf(
                buffer,
                sizeof(buffer),
                "%lld",
                (long long)value->payload
            );
            assert(n > 0 && (size_t)n < sizeof(buffer));

            size_t length = (size_t)n;
            AsciiStrValue *result = arena_alloc(
                a,
                sizeof(AsciiStrValue) + length,
                alignof(AsciiStrValue)
            );
            result->tag = TAG_ASCII_STR;
            result->length_in_bytes = (u64)length;
            memcpy(result->payload, buffer, length);

            return (size_t)((unsigned char *)result - a->bytes);
        }
        case TAG_ASCII_STR:
            return offset;
        default:
            assert(0 && "unsupported type tag in generic_to_str");
            return 0; /* unreachable */
    }
}

