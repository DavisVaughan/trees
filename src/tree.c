#include "tree.h"

#include "Rinternals.h"
#include "external-pointer.h"
#include "node.h"
#include "tree_sitter/api.h"

void r_ts_tree_finalize(SEXP ptr) {
    if (TYPEOF(ptr) != EXTPTRSXP) {
        return;
    }

    TSTree* tree = (TSTree*) R_ExternalPtrAddr(ptr);

    if (tree == NULL) {
        return;
    }

    ts_tree_delete(tree);

    R_ClearExternalPtr(ptr);
}

SEXP r_ts_tree_as_external_pointer(TSTree* tree) {
    return r_ts_as_external_pointer((void*) tree, "tree", r_ts_tree_finalize);
}

TSTree* r_ts_tree_from_external_pointer(SEXP tree) {
    if (TYPEOF(tree) != EXTPTRSXP) {
        Rf_errorcall(R_NilValue, "`tree` must be an external pointer.");
    }

    TSTree* out = (TSTree*) R_ExternalPtrAddr(tree);

    if (out == NULL) {
        Rf_errorcall(R_NilValue, "`tree` must point to a `TSTree`.");
    }

    return out;
}

// TODO
// - Switch first argument names to `x`?
// - Add print methods using `ts_node_string()` (don't forget to free() it)
// - Add get child methods to navigate nodes
// - Node wrapper needs to contain the `tree`
SEXP ffi_r_ts_tree_root_node(SEXP ffi_tree) {
    TSTree* tree = r_ts_tree_from_external_pointer(ffi_tree);
    TSNode node = ts_tree_root_node(tree);
    return r_ts_node_as_raw(node);
}
