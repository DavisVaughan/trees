#ifndef TREES_EXTERNAL_POINTER_H
#define TREES_EXTERNAL_POINTER_H

#include "Rinternals.h"

SEXP r_ts_as_external_pointer(
    void* x, const char* type, void(fn_finalize)(SEXP)
);

#endif
