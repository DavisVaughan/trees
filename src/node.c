#include "node.h"

#include <string.h>

#include "Rinternals.h"
#include "tree_sitter/api.h"

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
