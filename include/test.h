#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

#include "typedefs.h"

static i32 mt_run = 0;
static i32 mt_passed = 0;
static i32 mt_failed = 0;

#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf( \
            stderr, \
            "    ASSERT failed at %s:%d: %s\n", \
            __FILE__, \
            __LINE__, \
            #cond \
        ); \
        mt_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_EQ(actual, expected) do { \
    i64 expected_value = (expected); \
    i64 actual_value = (actual); \
    if (expected_value != actual_value) { \
        fprintf( \
            stderr, \
            "    ASSERT_EQ failed at %s:%d: expected %lld, got %lld\n", \
            __FILE__, \
            __LINE__, \
            expected_value, \
            actual_value \
        ); \
        mt_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_STR_EQ(actual, expected) do { \
    if (strcmp((expected), (actual)) != 0) { \
        fprintf( \
            stderr, \
            "    ASSERT_STR_EQ failed at %s:%d:\n" \
            "      expected: \"%s\"\n      actual:   \"%s\"\n", \
            __FILE__, \
            __LINE__, \
            (expected), \
            (actual) \
        ); \
        mt_failed++; \
        return; \
    } \
} while (0)

#define RUN_TEST(name) do { \
    int _fails_before = mt_failed; \
    printf("  %-50s ", #name); \
    name(); \
    mt_run++; \
    if (mt_failed == _fails_before) { \
        printf("[PASS]\n"); \
        mt_passed++; \
    } else { \
        printf("[FAIL]\n"); \
    } \
} while (0)

#define TEST_SUMMARY() do { \
    if (mt_failed > 0) { \
        printf( \
            "%d test%s failed\n\n", \
            mt_failed, \
            mt_failed > 1 ? "s" : "" \
        ); \
    } else { \
        printf("All tests passed\n\n"); \
    } \
    return mt_failed > 0 ? 1 : 0; \
} while (0)

#endif

