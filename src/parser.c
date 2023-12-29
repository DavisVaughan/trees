#include <stdint.h>

#include "R_ext/Boolean.h"
#include "Rinternals.h"
#include "external-pointer.h"
#include "language.h"
#include "tree.h"
#include "tree_sitter/api.h"

void r_ts_parser_finalize(SEXP ptr) {
    if (TYPEOF(ptr) != EXTPTRSXP) {
        return;
    }

    struct TSParser* parser = (struct TSParser*) R_ExternalPtrAddr(ptr);

    if (parser == NULL) {
        return;
    }

    ts_parser_delete(parser);

    R_ClearExternalPtr(ptr);
}

SEXP r_ts_parser_as_external_pointer(struct TSParser* parser) {
    return r_ts_as_external_pointer(
        (void*) parser, "parser", r_ts_parser_finalize
    );
}

struct TSParser* r_ts_parser_from_external_pointer(SEXP parser) {
    if (TYPEOF(parser) != EXTPTRSXP) {
        Rf_errorcall(R_NilValue, "`parser` must be an external pointer.");
    }

    struct TSParser* out = (struct TSParser*) R_ExternalPtrAddr(parser);

    if (out == NULL) {
        Rf_errorcall(R_NilValue, "`parser` must point to a `TSParser`.");
    }

    return out;
}

SEXP ffi_r_ts_new_parser(void) {
    struct TSParser* parser = ts_parser_new();
    return r_ts_parser_as_external_pointer(parser);
}

SEXP ffi_r_ts_parser_set_language(SEXP ffi_parser, SEXP ffi_language) {
    struct TSParser* parser = r_ts_parser_from_external_pointer(ffi_parser);

    const TSLanguage* language =
        r_ts_language_from_external_pointer(ffi_language);

    bool status = ts_parser_set_language(parser, language);

    SEXP out = PROTECT(Rf_allocVector(LGLSXP, 1));
    SET_LOGICAL_ELT(out, 0, (int) status);

    UNPROTECT(1);
    return out;
}

SEXP ffi_r_ts_parser_get_language(SEXP ffi_parser) {
    struct TSParser* parser = r_ts_parser_from_external_pointer(ffi_parser);

    const TSLanguage* language = ts_parser_language(parser);

    if (language == NULL) {
        return R_NilValue;
    }

    return r_ts_language_as_external_pointer(language);
}

SEXP ffi_r_ts_parser_parse(SEXP ffi_parser, SEXP ffi_code) {
    struct TSParser* parser = r_ts_parser_from_external_pointer(ffi_parser);

    SEXP code_char = STRING_ELT(ffi_code, 0);
    const char* code = CHAR(code_char);
    uint32_t size = (uint32_t) Rf_xlength(code_char);

    TSTree* tree = ts_parser_parse_string(parser, NULL, code, size);

    return r_ts_tree_as_external_pointer(tree);
}
