.onLoad <- function(libname, pkgname) {
    ns <- asNamespace(pkgname)
    initialize(ns)
    run_on_load()
}

initialize <- function(ns) {
    .Call(ffi_initialize, ns)
}
