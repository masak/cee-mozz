#include <stdio.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/array-value.h"
#include "../include/ascii-str-value.h"
#include "../include/code-table.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/func-value.h"
#include "../include/i64-value.h"
#include "../include/int-table.h"
#include "../include/macro-value.h"
#include "../include/str-table.h"
#include "../include/value.h"
#include "../include/tags.h"

Tag value_tag(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(Value));
    return ((Value *)(a->bytes + offset))->tag;
}

bool generic_validate(Arena *a, Offset offset, SeenSet *seenset) {
    switch (value_tag(a, offset)) {
        case TAG_I64:
            return i64_validate(a, offset, seenset);
        case TAG_ASCII_STR:
            return ascii_str_validate(a, offset, seenset);
        /* skipping bool 0x03, none 0x04, uninitialized 0x05 for now */
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
        case TAG_INT_TABLE:
            return inttable_validate(a, offset, seenset);
        case TAG_STR_TABLE:
            return strtable_validate(a, offset, seenset);
        case TAG_CODE_TABLE:
            return codetable_validate(a, offset, seenset);
        default:
            assert(0 && "unsupported type tag in generic_validate");
            return 0; /* unreachable */
    }
}

/* Convert any Value to its AsciiStrValue representation */
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

