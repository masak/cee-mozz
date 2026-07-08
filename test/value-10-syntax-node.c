#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/array-value.h"
#include "../include/ascii-str-value.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/syntax-node-value.h"
#include "../include/tags.h"
#include "../include/test.h"
#include "../include/typedefs.h"
#include "../include/value.h"

#define ASSERT_SYNTAX_NODE_NEW(kind, payload, cco, children) do { \
    Arena arena; \
    arena_init(&arena); \
    s8 kind_str = s8(kind); \
    Offset kind_offset = ascii_str_new(&arena, &kind_str); \
    Offset node = syntax_node_new( \
        &arena, \
        kind_offset, \
        payload, \
        (cco), \
        (children) \
    ); \
    SyntaxNodeValue *val = syntax_node_resolve(&arena, node); \
    ASSERT_TAG_EQ(val->tag, TAG_SYNTAX_NODE); \
    ASSERT_OFFSET_EQ(val->kind_offset, kind_offset); \
    ASSERT_U32_EQ(val->child_count, (cco)); \
} while (0)

void create_leaf_node(void) {
    ASSERT_SYNTAX_NODE_NEW("identifier", UNSET, 0, NULL);
}

void create_node_with_payload(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("IntNode");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset payload = i64_new(&arena, 42);
    Offset node = syntax_node_new(&arena, kind_offset, payload, 0, NULL);
    SyntaxNodeValue *val = syntax_node_resolve(&arena, node);
    ASSERT_TAG_EQ(val->tag, TAG_SYNTAX_NODE);
    ASSERT_OFFSET_EQ(val->kind_offset, kind_offset);
    ASSERT_OFFSET_EQ(val->payload_offset, payload);
    ASSERT_U32_EQ(val->child_count, 0);
}

void create_node_with_children(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("InfixOpExpr");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    s8 left_kind = s8("IntLitExpr");
    Offset left_kind_offset = ascii_str_new(&arena, &left_kind);
    Offset left_payload = i64_new(&arena, 3);
    Offset left = syntax_node_new(
        &arena,
        left_kind_offset,
        left_payload,
        0,
        NULL
    );
    s8 right_kind = s8("IntLitExpr");
    Offset right_kind_offset = ascii_str_new(&arena, &right_kind);
    Offset right_payload = i64_new(&arena, 3);
    Offset right = syntax_node_new(
        &arena,
        right_kind_offset,
        right_payload,
        0,
        NULL
    );
    Offset children[] = { left, right };
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 2, children);
    SyntaxNodeValue *val = syntax_node_resolve(&arena, node);
    ASSERT_TAG_EQ(val->tag, TAG_SYNTAX_NODE);
    ASSERT_U32_EQ(val->child_count, 2);
    ASSERT_OFFSET_EQ(val->children[0], left);
    ASSERT_OFFSET_EQ(val->children[1], right);
}

void kind_is_ascii_str(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("foo");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 0, NULL);
    Offset retrieved = syntax_node_kind(&arena, node);
    ASSERT_OFFSET_EQ(retrieved, kind_offset);
}

void payload_roundtrip(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("StrLitExpr");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    s8 payload_str = s8("hello");
    Offset payload_offset = ascii_str_new(&arena, &payload_str);
    Offset node = syntax_node_new(
        &arena,
        kind_offset,
        payload_offset,
        0,
        NULL
    );
    Offset retrieved = syntax_node_payload(&arena, node);
    ASSERT_OFFSET_EQ(retrieved, payload_offset);
}

void no_payload_is_unset(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("Identifier");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 0, NULL);
    Offset retrieved = syntax_node_payload(&arena, node);
    ASSERT_OFFSET_EQ(retrieved, UNSET);
}

void child_count_zero(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("NoneLitExpr");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 0, NULL);
    u32 count = syntax_node_child_count(&arena, node);
    ASSERT_U32_EQ(count, 0);
}

void child_count_multiple(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("Block");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    s8 stmt_kind = s8("Statement");
    Offset stmt_kind_offset = ascii_str_new(&arena, &stmt_kind);
    Offset stmt1 = syntax_node_new(&arena, stmt_kind_offset, UNSET, 0, NULL);
    Offset stmt2 = syntax_node_new(&arena, stmt_kind_offset, UNSET, 0, NULL);
    Offset stmt3 = syntax_node_new(&arena, stmt_kind_offset, UNSET, 0, NULL);
    Offset children[] = { stmt1, stmt2, stmt3 };
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 3, children);
    u32 count = syntax_node_child_count(&arena, node);
    ASSERT_U32_EQ(count, 3);
}

void get_child_by_index(void) {
    Arena arena;
    arena_init(&arena);
    s8 kind_str = s8("Pair");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    s8 first_kind = s8("First");
    Offset first_kind_offset = ascii_str_new(&arena, &first_kind);
    Offset first = syntax_node_new(&arena, first_kind_offset, UNSET, 0, NULL);
    s8 second_kind = s8("First");
    Offset second_kind_offset = ascii_str_new(&arena, &second_kind);
    Offset second = syntax_node_new(
        &arena,
        second_kind_offset,
        UNSET,
        0,
        NULL
    );
    Offset children[] = { first, second };
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 2, children);
    Offset retrieved_first = syntax_node_child(&arena, node, 0);
    Offset retrieved_second = syntax_node_child(&arena, node, 1);
    ASSERT_OFFSET_EQ(retrieved_first, first);
    ASSERT_OFFSET_EQ(retrieved_second, second);
}

void validate_leaf_node(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    s8 kind_str = s8("leaf");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 0, NULL);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(valid);
}

void validate_node_with_children(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    s8 kind_str = s8("InfixOpExpr");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    s8 child_kind = s8("IntLitExpr");
    Offset child_kind_offset = ascii_str_new(&arena, &child_kind);
    Offset child1 = syntax_node_new(
        &arena,
        child_kind_offset,
        i64_new(&arena, 1),
        0,
        NULL
    );
    Offset child2 = syntax_node_new(
        &arena,
        child_kind_offset,
        i64_new(&arena, 2),
        0,
        NULL
    );
    Offset children[] = { child1, child2 };
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 2, children);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(valid);
}

void validate_detects_bad_kind_tag(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    Offset bad_kind = i64_new(&arena, 42);
    Offset node = syntax_node_new(&arena, bad_kind, UNSET, 0, NULL);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(!valid);
}

void validate_detects_unset_kind(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    Offset node = syntax_node_new(&arena, UNSET, UNSET, 0, NULL);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(!valid);
}

void validate_detects_bad_child_tag(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    s8 kind_str = s8("InfixOpExpr");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset bad_child = i64_new(&arena, 42);
    Offset children[] = { bad_child };
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 1, children);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(!valid);
}

void validate_detects_unset_child(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    s8 kind_str = s8("InfixOpExpr");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    Offset children[] = { UNSET };
    Offset node = syntax_node_new(&arena, kind_offset, UNSET, 1, children);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(!valid);
}

void validate_detects_bad_payload_tag(void) {
    Arena arena;
    arena_init(&arena);
    SeenSet seenset;
    seenset_init(&seenset);
    s8 kind_str = s8("foo");
    Offset kind_offset = ascii_str_new(&arena, &kind_str);
    /* ArrayValue is not a valid payload type */
    Offset bad_payload = array_new(&arena, 4);
    Offset node = syntax_node_new(&arena, kind_offset, bad_payload, 0, NULL);
    bool valid = syntax_node_validate(&arena, node, &seenset);
    ASSERT_TRUE(!valid);
}

int main(void) {
    PLAN(16);
    RUN_TEST(create_leaf_node);
    RUN_TEST(create_node_with_payload);
    RUN_TEST(create_node_with_children);
    RUN_TEST(kind_is_ascii_str);
    RUN_TEST(payload_roundtrip);
    RUN_TEST(no_payload_is_unset);
    RUN_TEST(child_count_zero);
    RUN_TEST(child_count_multiple);
    RUN_TEST(get_child_by_index);
    RUN_TEST(validate_leaf_node);
    RUN_TEST(validate_node_with_children);
    RUN_TEST(validate_detects_bad_kind_tag);
    RUN_TEST(validate_detects_unset_kind);
    RUN_TEST(validate_detects_bad_child_tag);
    RUN_TEST(validate_detects_unset_child);
    RUN_TEST(validate_detects_bad_payload_tag);
    TEST_SUMMARY();
}

