#include <stdbool.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/seenset.h"
#include "../include/str-value.h"
#include "../include/tags.h"
#include "../include/typedefs.h"
#include "../include/value.h"

/* Validate a UTF-8 byte sequence and count its code points.
   Returns true if well-formed, and writes the count to *out_count. */
static bool utf8_validate_and_count(u8 *bytes, u32 length, u32 *out_count) {
    u32 count = 0;
    u32 i = 0;

    while (i < length) {
        u8 b = bytes[i];
        u32 seq_len;
        u32 min_val, max_val;

        if (b < 0x80) {
            seq_len = 1;
            min_val = 0x00;
            max_val = 0x7F;
        }
        else if ((b & 0xE0) == 0xC0) {
            seq_len = 2;
            min_val = 0x80;
            max_val = 0x7FF;
        }
        else if ((b & 0xF0) == 0xE0) {
            seq_len = 3;
            min_val = 0x800;
            max_val = 0xFFFF;
        }
        else if ((b & 0xF8) == 0xF0) {
            seq_len = 4;
            min_val = 0x10000;
            max_val = 0x10FFFF;
        }
        else {
            return false;  /* invalid leading byte */
        }

        if (i + seq_len > length) {
            return false;  /* truncated sequence */
        }

        u32 codepoint = b & (0xFF >> seq_len);
        for (u32 j = 1; j < seq_len; j++) {
            u8 c = bytes[i + j];
            if ((c & 0xC0) != 0x80) {
                return false;
            }
            codepoint = (codepoint << 6) | (c & 0x3F);
        }

        /* Reject overlong forms and out-of-range values */
        if (codepoint < min_val || codepoint > max_val) {
            return false;
        }

        /* Reject UTF-16 surrogate halves */
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
            return false;
        }

        ++count;
        i += seq_len;
    }

    *out_count = count;
    return true;
}

Offset str_new(Arena *a, s8 *str) {
    u32 length_in_bytes = (u32)str->length_in_bytes;
    u32 length_in_codepoints;

    assert(utf8_validate_and_count(
        str->payload,
        length_in_bytes,
        &length_in_codepoints
    ));

    StrValue *str_value = arena_alloc(
        a,
        sizeof(StrValue) + length_in_bytes,
        alignof(StrValue)
    );
    str_value->tag = TAG_STR;
    str_value->length_in_bytes = length_in_bytes;
    str_value->length_in_codepoints = length_in_codepoints;
    memcpy(str_value->payload, str->payload, length_in_bytes);

    return (Offset)((unsigned char *)str_value - a->bytes);
}

StrValue *str_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(StrValue));
    return (StrValue *)(a->bytes + offset);
}

bool str_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    StrValue *value = str_resolve(a, offset);

    assert(
        offset + sizeof(StrValue) + value->length_in_bytes <= ARENA_SIZE
    );

    u32 count;
    if (!utf8_validate_and_count(
            (u8 *)value->payload,
            value->length_in_bytes,
            &count)) {
        return false;
    }

    if (count != value->length_in_codepoints) {
        return false;
    }

    return true;
}

Offset str_concat(Arena *a, Offset offset1, Offset offset2) {
    StrValue *lhs = str_resolve(a, offset1);
    StrValue *rhs = str_resolve(a, offset2);

    u32 length_in_bytes = lhs->length_in_bytes + rhs->length_in_bytes;
    u32 length_in_codepoints
        = lhs->length_in_codepoints + rhs->length_in_codepoints;
    u32 total_size = sizeof(StrValue) + length_in_bytes;

    StrValue *result = arena_alloc(a, total_size, alignof(StrValue));
    result->tag = TAG_STR;
    result->length_in_bytes = length_in_bytes;
    result->length_in_codepoints = length_in_codepoints;

    memcpy(result->payload, lhs->payload, lhs->length_in_bytes);
    memcpy(
        result->payload + lhs->length_in_bytes,
        rhs->payload,
        rhs->length_in_bytes
    );

    return (Offset)((unsigned char *)result - a->bytes);
}
