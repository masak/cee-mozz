#include "../include/arena.h"
#include "../include/array-value.h"
#include "../include/ascii-str-value.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/func-value.h"
#include "../include/i64-value.h"
#include "../include/macro-value.h"
#include "../include/value.h"
#include "../include/tags.h"

u64 value_tag(Arena *a, Offset offset) {
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
        default:
            assert(0 && "unsupported type tag in generic_validate");
            return 0; /* unreachable */
    }
}

