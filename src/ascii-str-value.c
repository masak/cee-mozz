#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/ascii-str-value.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/typedefs.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset ascii_str_new(Arena *a, s8 *str) {
    u32 length_in_bytes = str->length_in_bytes;
    AsciiStrValue *ascii_str_value = arena_alloc(
        a,
        sizeof(AsciiStrValue) + length_in_bytes,
        alignof(AsciiStrValue)
    );
    ascii_str_value->tag = TAG_ASCII_STR;
    ascii_str_value->length_in_bytes = length_in_bytes;
    memcpy(ascii_str_value->payload, str->payload, length_in_bytes);
    return (Offset)((unsigned char *)ascii_str_value - a->bytes);
}

AsciiStrValue *ascii_str_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(AsciiStrValue));
    return (AsciiStrValue *)(a->bytes + offset);
}

bool ascii_str_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    AsciiStrValue *value = ascii_str_resolve(a, offset);

    assert(
        offset + sizeof(AsciiStrValue) + value->length_in_bytes <= ARENA_SIZE
    );

    for (u32 i = 0; i < value->length_in_bytes; i++) {
        unsigned char c = (unsigned char)value->payload[i];
        if (c > 127) {
            return false;
        }
    }
    return true;
}

Offset ascii_str_concat(Arena *a, Offset offset1, Offset offset2) {
    AsciiStrValue *lhs = ascii_str_resolve(a, offset1);
    AsciiStrValue *rhs = ascii_str_resolve(a, offset2);

    u32 length_in_bytes = lhs->length_in_bytes + rhs->length_in_bytes;
    u32 total_size = sizeof(AsciiStrValue) + length_in_bytes;

    AsciiStrValue *result = arena_alloc(a, total_size, alignof(AsciiStrValue));
    result->tag = TAG_ASCII_STR;
    result->length_in_bytes = length_in_bytes;

    memcpy(result->payload, lhs->payload, lhs->length_in_bytes);
    memcpy(
        result->payload + lhs->length_in_bytes,
        rhs->payload,
        rhs->length_in_bytes
    );

    return (Offset)((unsigned char *)result - a->bytes);
}

/* Convert any supported value to its AsciiStrValue representation */
Offset generic_to_str(Arena *a, Offset offset) {
    switch (value_tag(a, offset)) {
        case TAG_I64: {
            I64Value *value = i64_resolve(a, offset);

            /* i64 range: -9223372036854775808 .. 9223372036854775807
             * maximum string length is 20 characters (19 digits + sign). */
            char buffer[32];
            int n = snprintf(
                buffer,
                sizeof(buffer),
                "%" PRId64,
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
            result->length_in_bytes = (u32)length;
            memcpy(result->payload, buffer, length);

            return (Offset)((unsigned char *)result - a->bytes);
        }
        case TAG_ASCII_STR:
            return offset;
        default:
            assert(0 && "unsupported type tag in generic_to_str");
            return 0; /* unreachable */
    }
}

