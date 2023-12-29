#include "external-pointer.h"

SEXP r_ts_as_external_pointer(
    void* x, const char* type, void(fn_finalize)(SEXP)
) {
    SEXP out = PROTECT(R_MakeExternalPtr(x, R_NilValue, R_NilValue));

    const Rboolean finalize_on_exit = TRUE;
    R_RegisterCFinalizerEx(out, fn_finalize, finalize_on_exit);

    Rf_setAttrib(out, Rf_install("type"), Rf_mkString(type));

    UNPROTECT(1);
    return out;
}
