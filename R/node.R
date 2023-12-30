new_node <- function(raw, tree) {
    edit_count <- tree$edit_count()

    out <- list(
        raw = raw,
        tree = tree,
        tree_edit_count = edit_count
    )

    class(out) <- "trees_node"

    out
}

node_raw <- function(x) {
    .subset2(x, "raw")
}

node_tree <- function(x) {
    .subset2(x, "tree")
}

node_tree_edit_count <- function(x) {
    .subset2(x, "tree_edit_count")
}

node_as_text <- function(x) {
    check_node(x)
    x <- node_raw(x)
    .Call(ffi_r_ts_node_as_text, x)
}

check_node <- function(x) {
    stopifnot(is_node(x))
}

is_node <- function(x) {
    inherits(x, "trees_node")
}
