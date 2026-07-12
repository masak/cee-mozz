#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/outcome.h"
#include "../include/seenset.h"
#include "../include/syntax-node-value.h"
#include "../include/tags.h"
#include "../include/value.h"

/* Allocate a new SyntaxNodeValue.
 *
 * Precondition: `kind_offset` points to a string.
 */
Offset syntax_node_new(
    Arena *a,
    Offset kind_offset,
    MaybeOffset payload_offset,
    u32 child_count,
    Offset children[]
) {
    size_t children_size = child_count * sizeof(Offset);
    SyntaxNodeValue *node = arena_alloc(
        a,
        sizeof(SyntaxNodeValue) + children_size,
        alignof(SyntaxNodeValue)
    );
    node->tag = TAG_SYNTAX_NODE;
    node->kind_offset = kind_offset;
    node->payload_offset = payload_offset;
    node->child_count = child_count;
    if (children != NULL) {
        memcpy(node->children, children, children_size);
    }

    return (Offset)((unsigned char *)node - a->bytes);
}

/* Return a pointer to a SyntaxNodeValue, given an offset into an arena.
 *
 * Precondition: `offset` points to a SyntaxNodeValue.
 */
SyntaxNodeValue *syntax_node_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_SYNTAX_NODE) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(SyntaxNodeValue));
    return (SyntaxNodeValue *)(a->bytes + offset);
}

bool syntax_node_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    SyntaxNodeValue *node = syntax_node_resolve(a, offset);

    Offset total_size =
        sizeof(SyntaxNodeValue) + node->child_count * sizeof(Offset);
    if (offset + total_size > ARENA_SIZE) {
        return false;
    }

    /* kind must be a string */
    Offset kind_offset = node->kind_offset;
    if (kind_offset == UNSET) {
        return false;
    }
    Tag kind_tag = value_tag(a, kind_offset);
    if (kind_tag != TAG_ASCII_STR && kind_tag != TAG_STR) {
        return false;
    }
    if (!generic_validate(a, kind_offset, seenset)) {
        return false;
    }

    /* payload, if present, must be integer, string, or boolean */
    MaybeOffset payload_offset = node->payload_offset;
    if (payload_offset != UNSET) {
        Tag payload_tag = value_tag(a, payload_offset);
        if (payload_tag != TAG_I64 && payload_tag != TAG_INT &&
            payload_tag != TAG_ASCII_STR && payload_tag != TAG_STR &&
            payload_tag != TAG_BOOL) {
            return false;
        }
        if (!generic_validate(a, payload_offset, seenset)) {
            return false;
        }
    }

    /* children must be SyntaxNodeValues */
    for (u32 i = 0; i < node->child_count; i++) {
        Offset child_offset = node->children[i];
        if (child_offset == UNSET) {
            return false;
        }
        if (value_tag(a, child_offset) != TAG_SYNTAX_NODE) {
            return false;
        }
        if (!syntax_node_validate(a, child_offset, seenset)) {
            return false;
        }
    }

    return true;
}

/* Return the kind string offset of the SyntaxNodeValue at `offset`.
 *
 * Precondition: `offset` points to a valid SyntaxNodeValue.
 */
Outcome syntax_node_kind(Arena *a, Offset offset, Offset *out_offset) {
    SyntaxNodeValue *node = syntax_node_resolve(a, offset);
    *out_offset = node->kind_offset;
    return OUTCOME_OK;
}

/* Return the (possibly UNSET) payload offset of the SyntaxNodeValue at
 * `offset`.
 *
 * Precondition: `offset` points to a valid SyntaxNodeValue.
 */
Outcome syntax_node_payload(Arena *a, Offset offset, MaybeOffset *out_offset) {
    SyntaxNodeValue *node = syntax_node_resolve(a, offset);
    *out_offset = node->payload_offset;
    return OUTCOME_OK;
}

/* Return the number of children of the SyntaxNodeValue at `offset`.
 *
 * Precondition: `offset` points to a valid SyntaxNodeValue.
 */
u32 syntax_node_child_count(Arena *a, Offset offset) {
    SyntaxNodeValue *node = syntax_node_resolve(a, offset);
    return node->child_count;
}

/* Return the child at `index` of the SyntaxNodeValue at `offset`.
 *
 * Precondition: `offset` points to a valid SyntaxNodeValue.
 * Additional expectation: `index` is within bounds.
 */
Outcome syntax_node_child(
    Arena *a,
    Offset offset,
    u32 index,
    Offset *out_offset
) {
    SyntaxNodeValue *node = syntax_node_resolve(a, offset);

    if (index >= node->child_count) {
        return OUTCOME_E604_INDEX;
    }

    *out_offset = node->children[index];
    return OUTCOME_OK;
}

