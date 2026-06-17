#include <string.h>

#include "../include/arena.h"
#include "../include/array-value.h"
#include "../include/ascii-str-value.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/func-value.h"
#include "../include/i64-value.h"
#include "../include/int-table.h"
#include "../include/macro-value.h"
#include "../include/parameters.h"
#include "../include/seenset.h"
#include "../include/str-table.h"
#include "../include/tags.h"
#include "../include/test.h"
#include "../include/typedefs.h"
#include "../include/value.h"

static Arena arena;

#define ASSERT_ARRAY_LENGTH(arr, expected) do{ \
    Offset len_offset = array_length(&arena, arr); \
    i64 actual = i64_resolve(&arena, len_offset)->payload; \
    ASSERT_I64_EQ(actual, expected); \
} while (0)

void create_func_with_empty_env_and_codeunit(void) {
    arena_init(&arena);
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset func_offset = func_new(&arena, env_offset, codeunit_offset);
    FuncValue *func = func_resolve(&arena, func_offset);

    ASSERT_TAG_EQ(func->tag, TAG_FUNC);
    ASSERT_OFFSET_EQ(func->env_offset, env_offset);
    ASSERT_OFFSET_EQ(func->codeunit_offset, codeunit_offset);
}

void validate_func(void) {
    arena_init(&arena);
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset func_offset = func_new(&arena, env_offset, codeunit_offset);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(func_validate(&arena, func_offset, &seenset));
}

void func_validate_with_populated_env(void) {
    arena_init(&arena);
    Offset i64_offset = i64_new(&arena, 42);
    EnvEntry entries[] = {
        { .writable = 1, .cell = i64_offset }
    };
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        1,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset env_offset = environment_new(&arena, UNSET, 1, entries);
    Offset func_offset = func_new(&arena, env_offset, codeunit_offset);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(func_validate(&arena, func_offset, &seenset));
}

void func_validate_with_nested_env(void) {
    arena_init(&arena);
    Offset i64_offset = i64_new(&arena, 100);
    EnvEntry outer_entries[] = {
        { .writable = 1, .cell = i64_offset }
    };
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset outer_env_offset = environment_new(&arena, UNSET, 1, outer_entries);
    Offset inner_env_offset
        = environment_new(&arena, outer_env_offset, 0, NULL);
    Offset func_offset = func_new(&arena, inner_env_offset, codeunit_offset);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(func_validate(&arena, func_offset, &seenset));
}

void func_validate_invalid_env_tag(void) {
    arena_init(&arena);
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset bad_env = i64_new(&arena, 42);
    Offset func_offset = func_new(&arena, bad_env, codeunit_offset);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!func_validate(&arena, func_offset, &seenset));
}

void func_validate_unset_env(void) {
    arena_init(&arena);
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset func_offset = func_new(&arena, UNSET, codeunit_offset);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!func_validate(&arena, func_offset, &seenset));
}

void func_validate_invalid_codeunit_tag(void) {
    arena_init(&arena);
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset bad_codeunit = i64_new(&arena, 42);
    Offset func_offset = func_new(&arena, env_offset, bad_codeunit);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!func_validate(&arena, func_offset, &seenset));
}

void func_validate_unset_codeunit(void) {
    arena_init(&arena);
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset func_offset = func_new(&arena, env_offset, UNSET);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!func_validate(&arena, func_offset, &seenset));
}

void func_validate_invalid_codeunit_state(void) {
    arena_init(&arena);
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset a_offset = ascii_str_new(&arena, &s8("a"));
    Offset b_offset = ascii_str_new(&arena, &s8("b"));
    Offset c_offset = ascii_str_new(&arena, &s8("c"));
    Offset d_offset = ascii_str_new(&arena, &s8("d"));
    Offset e_offset = ascii_str_new(&arena, &s8("e"));
    Offset params_offsets[] = {
        a_offset,
        b_offset,
        c_offset,
        d_offset,
        e_offset
    };
    Offset params_offset = parameters_new(&arena, 5, params_offsets);
    /* parameter_count > register_count is invalid */
    Offset bad_codeunit = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset func_offset = func_new(&arena, env_offset, bad_codeunit);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!func_validate(&arena, func_offset, &seenset));
}

void func_validate_codeunit_too_many_params(void) {
    arena_init(&arena);
    Offset x_offset = ascii_str_new(&arena, &s8("x"));
    Offset params_offsets[] = { /* 257 entries */
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset, x_offset, x_offset,
        x_offset, x_offset, x_offset, x_offset, x_offset
    };
    Offset params_offset = parameters_new(&arena, 0x101, params_offsets);
    Offset bad_codeunit = codeunit_new(
        &arena,
        params_offset,
        0x200,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset func_offset = func_new(&arena, env_offset, bad_codeunit);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!func_validate(&arena, func_offset, &seenset));
}

void func_create_distinct_value(void) {
    arena_init(&arena);
    Offset *params_offset = parameters_new(&arena, 0, NULL);
    Offset cu1 = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset env1 = environment_new(&arena, UNSET, 0, NULL);
    Offset func1 = func_new(&arena, env1, cu1);

    Offset cu2 = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset env2 = environment_new(&arena, UNSET, 0, NULL);
    Offset func2 = func_new(&arena, env2, cu2);

    ASSERT_OFFSET_NE(func1, func2);
    ASSERT_OFFSET_NE(func1, env1);
    ASSERT_OFFSET_NE(func1, cu1);
}

void func_env_contains_another_func(void) {
    arena_init(&arena);
    Offset inner_env = environment_new(&arena, UNSET, 0, NULL);
    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset inner_codeunit = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset inner_func = func_new(&arena, inner_env, inner_codeunit);

    EnvEntry entries[] = {
        { .writable = 1, .cell = inner_func }
    };
    Offset outer_env = environment_new(&arena, UNSET, 1, entries);
    Offset outer_codeunit = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        UNSET,
        UNSET,
        UNSET,
        0,
        NULL
    );
    Offset outer_func = func_new(&arena, outer_env, outer_codeunit);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(func_validate(&arena, outer_func, &seenset));
}

void func_codeunit_with_tables(void) {
    arena_init(&arena);
    Offset i64_1 = i64_new(&arena, 10);
    Offset i64_2 = i64_new(&arena, 20);
    Offset int_elems[] = { i64_1, i64_2 };
    Offset inttable = inttable_new(&arena, 2, int_elems);

    s8 str = s8("hello");
    Offset str_offset = ascii_str_new(&arena, &str);
    Offset str_elems[] = { str_offset };
    Offset strtable = strtable_new(&arena, 1,  str_elems);

    Offset params_offset = parameters_new(&arena, 0, NULL);
    Offset codeunit_offset = codeunit_new(
        &arena,
        params_offset,
        2,
        0,
        inttable,
        strtable,
        UNSET,
        0,
        NULL
    );
    Offset env_offset = environment_new(&arena, UNSET, 0, NULL);
    Offset func_offset = func_new(&arena, env_offset, codeunit_offset);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(func_validate(&arena, func_offset, &seenset));
}

int main(void) {
    PLAN(13);
    RUN_TEST(create_func_with_empty_env_and_codeunit);
    RUN_TEST(validate_func);
    RUN_TEST(func_validate_with_populated_env);
    RUN_TEST(func_validate_with_nested_env);
    RUN_TEST(func_validate_invalid_env_tag);
    RUN_TEST(func_validate_unset_env);
    RUN_TEST(func_validate_invalid_codeunit_tag);
    RUN_TEST(func_validate_unset_codeunit);
    RUN_TEST(func_validate_invalid_codeunit_state);
    RUN_TEST(func_validate_codeunit_too_many_params);
    RUN_TEST(func_create_distinct_value);
    RUN_TEST(func_env_contains_another_func);
    RUN_TEST(func_codeunit_with_tables);
    TEST_SUMMARY();
}

