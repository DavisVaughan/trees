#include <R.h>
#include <R_ext/Rdynload.h>
#include <Rinternals.h>
#include <stdlib.h>  // for NULL

#include "rlang.h"

r_obj* ffi_initialize(r_obj* ns) {
    r_init_library(ns);
    return r_null;
}

/* .Call calls */
extern r_obj* ffi_r_ts_language_get_pointer(r_obj*);
extern r_obj* ffi_r_ts_language_get_pointer_r_embedded(void);
extern r_obj* ffi_r_ts_language_version(r_obj*);
extern r_obj* ffi_r_ts_language_id_for_node_kind(r_obj*, r_obj*, r_obj*);
extern r_obj* ffi_r_ts_language_node_kind_for_id(r_obj*, r_obj*);
extern r_obj* ffi_r_ts_parser_initialize(r_obj*);
extern r_obj* ffi_r_ts_parser_parse(r_obj*, r_obj*);
extern r_obj* ffi_r_ts_tree_root_node(r_obj*);
extern r_obj* ffi_r_ts_node_is_named(r_obj*);
extern r_obj* ffi_r_ts_node_as_text(r_obj*);
extern r_obj* ffi_r_ts_node_child(r_obj*, r_obj*);
extern r_obj* ffi_r_ts_node_children(r_obj*, r_obj*);
extern r_obj* ffi_r_ts_node_named_children(r_obj*, r_obj*);

static const R_CallMethodDef CallEntries[] = {
    {"ffi_initialize", (DL_FUNC) &ffi_initialize, 1},
    {"ffi_r_ts_language_get_pointer", (DL_FUNC) &ffi_r_ts_language_get_pointer,
     1},
    {"ffi_r_ts_language_get_pointer_r_embedded",
     (DL_FUNC) &ffi_r_ts_language_get_pointer_r_embedded, 0},
    {"ffi_r_ts_language_version", (DL_FUNC) &ffi_r_ts_language_version, 1},
    {"ffi_r_ts_language_id_for_node_kind",
     (DL_FUNC) &ffi_r_ts_language_id_for_node_kind, 3},
    {"ffi_r_ts_language_node_kind_for_id",
     (DL_FUNC) &ffi_r_ts_language_node_kind_for_id, 2},
    {"ffi_r_ts_parser_initialize", (DL_FUNC) &ffi_r_ts_parser_initialize, 1},
    {"ffi_r_ts_parser_parse", (DL_FUNC) &ffi_r_ts_parser_parse, 2},
    {"ffi_r_ts_tree_root_node", (DL_FUNC) &ffi_r_ts_tree_root_node, 1},
    {"ffi_r_ts_node_is_named", (DL_FUNC) &ffi_r_ts_node_is_named, 1},
    {"ffi_r_ts_node_as_text", (DL_FUNC) &ffi_r_ts_node_as_text, 1},
    {"ffi_r_ts_node_child", (DL_FUNC) &ffi_r_ts_node_child, 2},
    {"ffi_r_ts_node_children", (DL_FUNC) &ffi_r_ts_node_children, 2},
    {"ffi_r_ts_node_named_children", (DL_FUNC) &ffi_r_ts_node_named_children,
     2},
    {NULL, NULL, 0}};

void R_init_trees(DllInfo* dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
