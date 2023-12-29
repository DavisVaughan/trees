#ifndef TREES_LANGUAGE_H
#define TREES_LANGUAGE_H

#include "rlang.h"
#include "tree_sitter/api.h"

const TSLanguage* r_ts_language_from_external_pointer(r_obj* language);
r_obj* r_ts_language_as_external_pointer(const TSLanguage* language);

#endif
