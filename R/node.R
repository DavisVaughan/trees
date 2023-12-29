node_as_text <- function(x) {
    .Call(ffi_r_ts_node_as_text, x)
}
