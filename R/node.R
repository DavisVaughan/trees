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

#' @export
print.trees_node <- function(x, ...) {
    cat(node_as_text(x))
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

node_is_named <- function(x) {
    check_node(x)
    x <- node_raw(x)
    .Call(ffi_r_ts_node_is_named, x)
}

node_as_text <- function(x) {
    check_node(x)
    x <- node_raw(x)
    .Call(ffi_r_ts_node_as_text, x)
}

# 0 indexed child
node_child <- function(x, index) {
    check_node(x)

    tree <- node_tree(x)
    x <- node_raw(x)

    out <- .Call(ffi_r_ts_node_child, x, index)

    if (is.null(out)) {
        out
    } else {
        new_node(out, tree)
    }
}

node_children <- function(x, ..., cursor = NULL) {
    check_dots_empty0(...)
    node_children_impl(x, cursor, ffi_r_ts_node_children)
}

node_named_children <- function(x, ..., cursor = NULL) {
    check_dots_empty0(...)
    node_children_impl(x, cursor, ffi_r_ts_node_named_children)
}

node_children_impl <- function(x, cursor, fn) {
    check_node(x)

    tree <- node_tree(x)
    x <- node_raw(x)

    out <- .Call(fn, x, cursor)

    for (i in seq_along(out)) {
        out[[i]] <- new_node(out[[i]], tree)
    }

    out
}

check_node <- function(x) {
    stopifnot(is_node(x))
}

is_node <- function(x) {
    inherits(x, "trees_node")
}
