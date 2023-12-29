#include "language.h"

#include <stdint.h>
#include <string.h>

#include "external-pointer.h"
#include "tree_sitter/api.h"

// From "tree-sitter-r/parse.c", through "tree-sitter-r.c"
extern const TSLanguage* tree_sitter_r_embedded(void);

typedef const TSLanguage* (*fn_ts_language_t)(void);

const TSLanguage* r_ts_language_get_pointer(const char* name) {
    // See if we can find the unique `tree_sitter_<language>()` function name
    fn_ts_language_t fn = (fn_ts_language_t) R_FindSymbol(name, "", NULL);

    // Supposedly R returns this when it can't find one
    if (!fn) {
        r_abort("Can't find function symbol with `name`: '%s'", name);
    }

    // Call it
    const TSLanguage* language = (fn) ();

    if (language == NULL) {
        r_abort("Function pointer did not evaluate to a `TSLanguage`.");
    }

    return language;
}

void r_ts_language_delete(r_obj* ptr) {
    if (r_typeof(ptr) != R_TYPE_pointer) {
        return;
    }

    const TSLanguage* language = (const TSLanguage*) R_ExternalPtrAddr(ptr);

    if (language == NULL) {
        return;
    }

    R_ClearExternalPtr(ptr);
}

r_obj* r_ts_language_as_external_pointer(const TSLanguage* language) {
    return r_ts_as_external_pointer(
        (void*) language, "language", r_ts_language_delete
    );
}

r_obj* ffi_r_ts_language_get_pointer(r_obj* ffi_name) {
    if (r_typeof(ffi_name) != R_TYPE_character) {
        r_abort("`name` must be a single string.");
    }
    if (r_length(ffi_name) != 1) {
        r_abort("`name` must be a single string.");
    }

    const char* name = r_chr_get_c_string(ffi_name, 0);

    const TSLanguage* language = r_ts_language_get_pointer(name);

    return r_ts_language_as_external_pointer(language);
}

r_obj* ffi_r_ts_language_get_pointer_r_embedded(void) {
    const TSLanguage* language = tree_sitter_r_embedded();
    return r_ts_language_as_external_pointer(language);
}

const TSLanguage* r_ts_language_from_external_pointer(r_obj* language) {
    if (r_typeof(language) != R_TYPE_pointer) {
        r_abort("`language` must be an external pointer.");
    }

    const TSLanguage* out = (const TSLanguage*) R_ExternalPtrAddr(language);

    if (out == NULL) {
        r_abort("`language` must point to a `TSLanguage`.");
    }

    return out;
}

r_obj* ffi_r_ts_language_version(r_obj* ffi_language) {
    const TSLanguage* language =
        r_ts_language_from_external_pointer(ffi_language);

    const uint32_t version = ts_language_version(language);

    r_obj* out = KEEP(r_alloc_integer(1));
    r_int_poke(out, 0, (int) version);

    FREE(1);
    return out;
}

r_obj* ffi_r_ts_language_id_for_node_kind(
    r_obj* ffi_language, r_obj* ffi_kind, r_obj* ffi_named
) {
    const TSLanguage* language =
        r_ts_language_from_external_pointer(ffi_language);

    r_obj* kind = r_chr_get(ffi_kind, 0);
    const char* v_kind = r_str_c_string(kind);
    uint32_t kind_length = (uint32_t) r_length(kind);

    const bool named = r_arg_as_bool(ffi_named, "named");

    TSSymbol id =
        ts_language_symbol_for_name(language, v_kind, kind_length, named);

    if (id == (TSSymbol) 0) {
        return r_null;
    }

    r_obj* out = KEEP(r_alloc_integer(1));
    r_int_poke(out, 0, (int) id);

    FREE(1);
    return out;
}

r_obj* ffi_r_ts_language_node_kind_for_id(r_obj* ffi_language, r_obj* ffi_id) {
    const TSLanguage* language =
        r_ts_language_from_external_pointer(ffi_language);

    // TSSymbol is `uint16` so an `int` mostly casts well
    const TSSymbol id = (TSSymbol) r_arg_as_int(ffi_id, "id");

    const char* name = ts_language_symbol_name(language, id);

    if (name == NULL) {
        return r_null;
    }

    r_obj* out = KEEP(r_alloc_character(1));
    r_chr_poke(out, 0, Rf_mkCharCE(name, CE_UTF8));

    FREE(1);
    return out;
}
