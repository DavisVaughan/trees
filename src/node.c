#include "node.h"

#include <string.h>

SEXP r_ts_node_as_raw(TSNode x) {
    // Unlike other tree-sitter objects, these aren't on the heap.
    // We represent nodes with raw vectors.
    // Lifetime management (i.e. tied to a tree) is done on the R side.
    SEXP out = PROTECT(Rf_allocVector(RAWSXP, sizeof(TSNode)));
    TSNode* p_out = (TSNode*) RAW(out);

    memcpy(p_out, &x, sizeof(TSNode));

    UNPROTECT(1);
    return out;
}

TSNode r_ts_node_from_raw(SEXP x) {
    if (TYPEOF(x) != RAWSXP) {
        Rf_errorcall(R_NilValue, "`x` must be a raw vector.");
    }

    TSNode* p_x = (TSNode*) RAW(x);

    // Local copy rather than returning `*p_x` and relying on that to exist
    TSNode out;
    memcpy(&out, p_x, sizeof(TSNode));

    return out;
}

r_obj* ffi_r_ts_node_is_named(r_obj* ffi_x) {
    const TSNode x = r_ts_node_from_raw(ffi_x);
    return r_lgl(ts_node_is_named(x));
}

SEXP ffi_r_ts_node_as_text(SEXP ffi_x) {
    TSNode x = r_ts_node_from_raw(ffi_x);

    char* c_text = ts_node_string(x);

    // Copy into R string so we can free it
    SEXP text = PROTECT(Rf_mkCharCE(c_text, CE_UTF8));

    free(c_text);

    SEXP out = PROTECT(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(out, 0, text);

    UNPROTECT(2);
    return out;
}

r_obj* ffi_r_ts_node_child(r_obj* ffi_x, r_obj* ffi_index) {
    const TSNode x = r_ts_node_from_raw(ffi_x);
    const uint32_t index = (uint32_t) r_arg_as_ssize(ffi_index, "index");

    const TSNode child = ts_node_child(x, index);

    if (ts_node_is_null(child)) {
        return r_null;
    } else {
        return r_ts_node_as_raw(child);
    }
}

r_obj* ffi_r_ts_node_children(r_obj* ffi_x, r_obj* ffi_cursor) {
    const TSNode x = r_ts_node_from_raw(ffi_x);

    // TODO: Support user supplied cursor, reset it to `x`
    if (ffi_cursor != r_null) {
        r_abort("Only `NULL` is supported for `cursor` right now.");
    }

    const uint32_t count = ts_node_child_count(x);

    r_obj* out = KEEP(r_alloc_list((r_ssize) count));

    TSTreeCursor cursor = ts_tree_cursor_new(x);
    ts_tree_cursor_goto_first_child(&cursor);

    for (uint32_t i = 0; i < count; ++i) {
        TSNode elt = ts_tree_cursor_current_node(&cursor);
        SET_VECTOR_ELT(out, i, r_ts_node_as_raw(elt));
        ts_tree_cursor_goto_next_sibling(&cursor);
    }

    FREE(1);
    return out;
}

r_obj* ffi_r_ts_node_named_children(r_obj* ffi_x, r_obj* ffi_cursor) {
    const TSNode x = r_ts_node_from_raw(ffi_x);

    // TODO: Support user supplied cursor, reset it to `x`
    if (ffi_cursor != r_null) {
        r_abort("Only `NULL` is supported for `cursor` right now.");
    }

    const uint32_t count = ts_node_child_count(x);
    const uint32_t out_count = ts_node_named_child_count(x);

    r_ssize i_out = 0;
    r_obj* out = KEEP(r_alloc_list((r_ssize) out_count));

    TSTreeCursor cursor = ts_tree_cursor_new(x);
    ts_tree_cursor_goto_first_child(&cursor);

    for (uint32_t i = 0; i < count; ++i) {
        TSNode elt = ts_tree_cursor_current_node(&cursor);

        if (!ts_node_is_named(elt)) {
            ts_tree_cursor_goto_next_sibling(&cursor);
            continue;
        }

        SET_VECTOR_ELT(out, i_out, r_ts_node_as_raw(elt));
        ++i_out;

        ts_tree_cursor_goto_next_sibling(&cursor);
    }

    FREE(1);
    return out;
}
