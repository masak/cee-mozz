#include <stdio.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/array-value.h"
#include "../include/ascii-str-value.h"
#include "../include/code-table.h"
#include "../include/codeunit.h"
#include "../include/crash.h"
#include "../include/environment.h"
#include "../include/func-value.h"
#include "../include/i64-value.h"
#include "../include/int-value.h"
#include "../include/int-table.h"
#include "../include/macro-value.h"
#include "../include/parameters.h"
#include "../include/str-table.h"
#include "../include/str-value.h"
#include "../include/value.h"
#include "../include/tags.h"

Tag value_tag(Arena *a, Offset offset) {
    if (offset >= ARENA_SIZE) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }
    return ((Value *)(a->bytes + offset))->tag;
}

bool generic_validate(Arena *a, Offset offset, SeenSet *seenset) {
    switch (value_tag(a, offset)) {
        case TAG_I64:
            return i64_validate(a, offset, seenset);
        case TAG_ASCII_STR:
            return ascii_str_validate(a, offset, seenset);
        /* skipping bool 0x03, none 0x04 for now */
        case TAG_ARRAY:
            return array_validate(a, offset, seenset);
        case TAG_ARRAY_ELEMENTS:
            return array_elements_validate(a, offset, seenset);
        case TAG_FUNC:
            return func_validate(a, offset, seenset);
        case TAG_MACRO:
            return macro_validate(a, offset, seenset);
        case TAG_ENVIRONMENT:
            return environment_validate(a, offset, seenset);
        case TAG_CODE_UNIT:
            return codeunit_validate(a, offset, seenset);
        case TAG_INT:
            return int_validate(a, offset, seenset);
        case TAG_STR:
            return str_validate(a, offset, seenset);
        case TAG_INT_TABLE:
            return inttable_validate(a, offset, seenset);
        case TAG_STR_TABLE:
            return strtable_validate(a, offset, seenset);
        case TAG_CODE_TABLE:
            return codetable_validate(a, offset, seenset);
        case TAG_PARAMETERS:
            return parameters_validate(a, offset, seenset);
        default:
            assert(0 && "unsupported type tag in generic_validate");
            return 0; /* unreachable */
    }
}

/* Convert any Value to its string representation */
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
                value->payload
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
        case TAG_INT:
            return int_to_str(a, offset);
        case TAG_STR:
            return offset;
        default:
            assert(0 && "unsupported type tag in generic_to_str");
            return 0; /* unreachable */
    }
}

/* XXX: Implement generic_string_concat() */

