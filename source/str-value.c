#include <stdbool.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/generic-string.h"
#include "../include/outcome.h"
#include "../include/seenset.h"
#include "../include/str-value.h"
#include "../include/tags.h"
#include "../include/typedefs.h"
#include "../include/value.h"

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

/* Return a pointer to a StrValue, given an offset into an arena.
 *
 * Precondition: `offset` points to a StrValue.
 */
StrValue *str_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_STR) {
        vm_crash(CRASH_INVALID_TAG);
    }

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

/* Return an StrValue which is the concatenation of `offset1` and `offset2`.
 *
 * Preconditions: `offset1` points to a valid StrValue; `offset2` points to a
 *                valid StrValue. Their combined length (in bytes) fits in a
 *                `u32`.
 */
Outcome str_concat(
    Arena *a,
    Offset offset1,
    Offset offset2,
    Offset *out_offset
) {
    StrValue *lhs = str_resolve(a, offset1);
    StrValue *rhs = str_resolve(a, offset2);

    u32 length_in_bytes = lhs->length_in_bytes + rhs->length_in_bytes;
    u32 length_in_codepoints
        = lhs->length_in_codepoints + rhs->length_in_codepoints;
    u32 total_size = sizeof(StrValue) + length_in_bytes;
    if (total_size < lhs->length_in_bytes) {
        vm_crash(CRASH_STRING_TOO_LONG);
    }

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

    *out_offset = (Offset)((unsigned char *)result - a->bytes);
    return OUTCOME_OK;
}

