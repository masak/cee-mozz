#ifndef SYNTAX_NODE_VALUE_H
#define SYNTAX_NODE_VALUE_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "arena.h"
#include "outcome.h"
#include "tags.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    Offset kind_offset;
    MaybeOffset payload_offset;
    u32 child_count;
    Offset children[];
} SyntaxNodeValue;

Offset syntax_node_new(
    Arena *a,
    Offset kind_offset,
    MaybeOffset payload_offset,
    u32 child_count,
    Offset children[]
);
SyntaxNodeValue *syntax_node_resolve(Arena *a, Offset offset);
bool syntax_node_validate(Arena *a, Offset offset, SeenSet *seenset);

Outcome syntax_node_kind(Arena *a, Offset offset, Offset *out_offset);
Outcome syntax_node_payload(Arena *a, Offset offset, MaybeOffset *out_offset);
u32 syntax_node_child_count(Arena *a, Offset offset);
Outcome syntax_node_child(
    Arena *a,
    Offset offset,
    u32 index,
    Offset *out_offset
);

#endif

