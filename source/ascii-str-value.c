#include <stdbool.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/ascii-str-value.h"
#include "../include/crash.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/typedefs.h"
#include "../include/tags.h"
#include "../include/value.h"

/* Allocate a new AsciiStrValue. */
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

/* Returns a pointer to an AsciiStrValue, given an offset into an arena.
 *
 * Precondition: `offset` points to an AsciiStrValue.
 */
AsciiStrValue *ascii_str_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_ASCII_STR) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(AsciiStrValue));
    return (AsciiStrValue *)(a->bytes + offset);
}

bool ascii_str_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    AsciiStrValue *value = ascii_str_resolve(a, offset);

    if (offset + sizeof(AsciiStrValue) + value->length_in_bytes > ARENA_SIZE) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }

    for (u32 i = 0; i < value->length_in_bytes; i++) {
        unsigned char c = (unsigned char)value->payload[i];
        if (c > 127) {
            return false;
        }
    }
    return true;
}

/* Return an AsciiStrValue which is the concatenation of `offset1` and
 * `offset2`.
 *
 * Preconditions: `offset1` points to a valid AsciiStrValue; `offset2` points
 *                to a valid AsciiStrValue. Their combined length fits in a
 *                `u32`.
 */
Offset ascii_str_concat(Arena *a, Offset offset1, Offset offset2) {
    AsciiStrValue *lhs = ascii_str_resolve(a, offset1);
    AsciiStrValue *rhs = ascii_str_resolve(a, offset2);

    u32 length_in_bytes = lhs->length_in_bytes + rhs->length_in_bytes;
    u32 total_size = sizeof(AsciiStrValue) + length_in_bytes;
    if (total_size < lhs->length_in_bytes) {
        vm_crash(CRASH_STRING_TOO_LONG);
    }

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

