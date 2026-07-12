#include <stdbool.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/generic-string.h"
#include "../include/seenset.h"
#include "../include/small-str-value.h"
#include "../include/str-value.h"
#include "../include/tags.h"
#include "../include/typedefs.h"
#include "../include/value.h"

/* Allocate a SmallStrValue.
 *
 * Preconditions: `str->length_in_bytes <= SMALL_STR_MAX_BYTES`. `str` contains
 *                well-formed UTF-8. The initial byte of `str` is not '\0'.
 */
Offset small_str_new(Arena *a, s8 *str) {
    u32 length_in_bytes = (u32)str->length_in_bytes;
    if (length_in_bytes > SMALL_STR_MAX_BYTES) {
        vm_crash(CRASH_STRING_TOO_LONG);
    }

    SmallStrValue *small_str_value = arena_alloc(
        a,
        sizeof(SmallStrValue),
        alignof(SmallStrValue)
    );
    small_str_value->tag = TAG_SMALL_STR;
    memset(small_str_value->payload, 0, SMALL_STR_MAX_BYTES);
    memcpy(
        small_str_value->payload + (SMALL_STR_MAX_BYTES - length_in_bytes),
        str->payload,
        length_in_bytes
    );
    return (Offset)((unsigned char *)small_str_value - a->bytes);
}

/* Return a pointer to a SmallStrValue.
 *
 * Precondition: `offset` points to a SmallStrValue.
 */
SmallStrValue *small_str_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_SMALL_STR) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(SmallStrValue));
    return (SmallStrValue *)(a->bytes + offset);
}

/* Extract the UTF-8 payload from a SmallStrValue into `out`.
 *
 * The payload is right-aligned in the 8-byte field; leading zero bytes are
 * padding, not part of the string. We scan from the left to find the first
 * non-zero byte, if any; everything from that point to the end is the payload.
 */
static void small_str_extract_payload(
    SmallStrValue *value,
    u8 *out_payload,
    u32 *out_length_in_bytes
) {
    u32 i = 0;
    while (i < SMALL_STR_MAX_BYTES && value->payload[i] == 0) {
        ++i;
    }
    u32 length_in_bytes = SMALL_STR_MAX_BYTES - i;
    memcpy(out_payload, value->payload + i, length_in_bytes);
    *out_length_in_bytes = length_in_bytes;
}

bool small_str_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    assert(offset <= ARENA_SIZE - sizeof(SmallStrValue));

    SmallStrValue *value = small_str_resolve(a, offset);

    u8 payload[SMALL_STR_MAX_BYTES];
    u32 length_in_bytes;
    small_str_extract_payload(value, payload, &length_in_bytes);

    u32 count;
    return utf8_validate_and_count(payload, length_in_bytes, &count);
}

/* Return a StrValue which is the concatenation of `offset1` and `offset2`.
 *
 * If the result fits in SMALL_STR_MAX_BYTES, a SmallStrValue is returned.
 * Otherwise, a StrValue is returned.
 *
 * Preconditions: `offset1` points to a valid SmallStrValue. `offset2` points
 *                to a valid SmallStrValue.
 * Additional expectation: The combined length of the two strings fits in a
 *                         `u32`.
 */
Offset small_str_concat(Arena *a, Offset offset1, Offset offset2) {
    u8 payload1[SMALL_STR_MAX_BYTES];
    u8 payload2[SMALL_STR_MAX_BYTES];
    u32 length1 = 0;
    u32 length2 = 0;

    if (value_tag(a, offset1) != TAG_SMALL_STR) {
        vm_crash(CRASH_INVALID_TAG);
    }
    if (value_tag(a, offset2) != TAG_SMALL_STR) {
        vm_crash(CRASH_INVALID_TAG);
    }

    SmallStrValue *v1 = small_str_resolve(a, offset1);
    small_str_extract_payload(v1, payload1, &length1);

    SmallStrValue *v2 = small_str_resolve(a, offset2);
    small_str_extract_payload(v2, payload2, &length2);

    u32 combined_length = length1 + length2;
    if (combined_length < length1) { /* u32 overflow */
        vm_crash(CRASH_STRING_TOO_LONG);
    }

    u8 combined[SMALL_STR_MAX_BYTES * 2];
    memcpy(combined, payload1, length1);
    memcpy(combined + length1, payload2, length2);
    s8 str = { combined, combined_length };

    if (combined_length <= SMALL_STR_MAX_BYTES) {
        return small_str_new(a, &str);
    }
    else {
        return str_new(a, &str);
    }
}
