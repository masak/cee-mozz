#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

#include "typedefs.h"

#ifdef _WIN32
#define MT_RED      ""
#define MT_GREEN    ""
#define MT_RESET    ""
#else
#define MT_RED      "\033[0;31m"
#define MT_GREEN    "\033[0;32m"
#define MT_RESET    "\033[0m"
#endif

static i32 mt_run = 0;
static i32 mt_passed = 0;
static i32 mt_failed = 0;

#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf( \
            stderr, \
            MT_RED "    ASSERT failed at %s:%d: %s" MT_RESET "\n", \
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
            MT_RED "    ASSERT_EQ failed at %s:%d: expected %lld, got %lld" \
            MT_RESET "\n", \
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
            MT_RED "    ASSERT_STR_EQ failed at %s:%d:\n" \
            "      expected: \"%s\"\n      actual:   \"%s\"" MT_RESET "\n", \
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
        printf(MT_GREEN "[PASS]" MT_RESET "\n"); \
        mt_passed++; \
    } else { \
        printf(MT_RED "[FAIL]" MT_RESET "\n"); \
    } \
} while (0)

#define TEST_SUMMARY() do { \
    printf( \
        "\n%s%d/%d tests passed%s\n", \
        mt_failed > 0 ? MT_RED : MT_GREEN, \
        mt_passed, \
        mt_run, \
        MT_RESET \
    ); \
    return mt_failed > 0 ? 1 : 0; \
} while (0)

#endif

