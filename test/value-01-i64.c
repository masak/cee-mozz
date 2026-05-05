#include "../include/i64-value.h"
#include "../include/test.h"

void test_add_i64s(void) {
    Arena arena;
    arena_init(&arena);

    size_t x = i64_new(&arena, 40);
    size_t y = i64_new(&arena, 2);
    size_t z = i64_add(&arena, x, y);

    i64 actual = i64_resolve(&arena, z)->payload;
    i64 expected = 42;

    ASSERT_EQ(actual, expected);
}

int main(void) {
    RUN_TEST(test_add_i64s);
    TEST_SUMMARY();
}

