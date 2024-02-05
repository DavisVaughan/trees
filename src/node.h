#ifndef TREES_NODE_H
#define TREES_NODE_H

#include "rlang.h"
#include "tree_sitter/api.h"

SEXP r_ts_node_as_raw(TSNode x);
TSNode r_ts_node_from_raw(SEXP x);

#endif
