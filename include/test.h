#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

#include "arena.h"
#include "typedefs.h"

/*
 * Lightweight TAP Test Framework
 *
 * Usage:
 *     #include "../include/test.h"
 *
 *     void test_something(void) {
 *         ASSERT_TRUE(1 + 1 == 2);
 *         ASSERT_I64_EQ(42, 42);
 *     }
 *
 *     int main(void) {
 *         TEST_PLAN(1);
 *         RUN_TEST(test_something);
 *         TEST_SUMMARY();
 *     }
 */

static u32 _test_total = 0;
static u32 _test_current = 0;
static u32 _test_passed = 0;
static u32 _test_failed = 0;
static u32 _plan_set = 0;

#define PLAN(n) do { \
    _test_total = (n); \
    _plan_set = 1; \
    printf("1..%d\n", (n)); \
} while (0)

static char _test_name_buf[256];

static const char* _format_test_name(const char* name) {
    size_t len = strlen(name);
    if (len >= sizeof(_test_name_buf)) {
        len = sizeof(_test_name_buf) - 1;
    }
    memcpy(_test_name_buf, name, len);
    _test_name_buf[len] = '\0';
    for (size_t i = 0; i < len; i++) {
        if (_test_name_buf[i] == '_') {
            _test_name_buf[i] = ' ';
        }
    }
    return _test_name_buf;
}

#define _TEST_RESULT(ok, name) do { \
    _test_current++; \
    const char* _display_name = _format_test_name(name); \
    if (ok) { \
        _test_passed++; \
        printf("ok %d - %s\n", _test_current, _display_name); \
    } \
    else { \
        _test_failed++; \
        printf("not ok %d - %s\n", _test_current, _display_name); \
    } \
} while (0)

/*
 * RUN_TEST(func) - Execute a test function and report its result.
 * The test function should use ASSERT_* macros; if any assertion fails,
 * the function should return early (via `return`).
 *
 * We track per-test failure with a flag. An ASSERT_* macro that fails sets
 * the failure flag and returns from the surrounding function.
 */
static u32 _current_test_failed = 0;

#define _MARK_TEST_FAILED() do { _current_test_failed = 1; } while (0)
#define _TEST_FAILED() (_current_test_failed != 0)
#define _RESET_TEST_STATE() do { _current_test_failed = 0; } while (0)

#define RUN_TEST(func) do { \
    _RESET_TEST_STATE(); \
    func(); \
    _TEST_RESULT(!_TEST_FAILED(), #func); \
} while (0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        printf("# ASSERT_TRUE failed: %s\n", #expr); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define ASSERT_I64_EQ(actual, expected) do { \
    i64 _act = (actual); \
    i64 _exp = (expected); \
    if (_act != _exp) { \
        printf("# ASSERT_I64_EQ failed\n"); \
        printf("#   actual:   %" PRId64 "\n", _act); \
        printf("#   expected: %" PRId64 "\n", _exp); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define ASSERT_U32_EQ(actual, expected) do { \
    u32 _act = (actual); \
    u32 _exp = (expected); \
    if (_act != _exp) { \
        printf("# ASSERT_U32_EQ failed\n"); \
        printf("#   actual:   %" PRIu32 "\n", _act); \
        printf("#   expected: %" PRIu32 "\n", _exp); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define ASSERT_TAG_EQ(actual, expected) do { \
    u32 _act = (actual); \
    u32 _exp = (expected); \
    if (_act != _exp) { \
        printf("# ASSERT_TAG_EQ failed\n"); \
        printf("#   actual:   %" PRIu32 "\n", _act); \
        printf("#   expected: %" PRIu32 "\n", _exp); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define ASSERT_OFFSET_EQ(actual, expected) do { \
    u32 _act = (actual); \
    u32 _exp = (expected); \
    if (_act != _exp) { \
        printf("# ASSERT_OFFSET_EQ failed\n"); \
        printf("#   actual:   %" PRIu32 "\n", _act); \
        printf("#   expected: %" PRIu32 "\n", _exp); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define ASSERT_OFFSET_NE(offset1, offset2) do { \
    u32 _o1 = (offset1); \
    u32 _o2 = (offset2); \
    if (_o1 == _o2) { \
        printf("# ASSERT_OFFSET_NE failed\n"); \
        printf("#   offset: %" PRIu32 "\n", _o1); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define ASSERT_STR_EQ(actual, expected) do { \
    if (strcmp(actual, expected) != 0) { \
        printf("# ASSERT_STR_EQ failed\n"); \
        printf("#   actual:   %s\n", (actual)); \
        printf("#   expected: %s\n", (expected)); \
        _MARK_TEST_FAILED(); \
        return; \
    } \
} while (0)

#define TEST_SUMMARY() do { \
    if (!_plan_set) { \
        fprintf(stderr, "# error: TEST_PLAN was not called\n"); \
        return 1; \
    } \
    if (_test_current != _test_total) { \
        fprintf( \
            stderr, \
            "# warning: planned %d tests but ran %d\n", \
            _test_total, \
            _test_current \
        ); \
    } \
    printf("\n"); \
    return (_test_failed > 0) ? 1 : 0; \
} while (0)

#endif

