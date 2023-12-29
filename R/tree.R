# Must be careful here. Node has to be tied to a `tree`.
tree_root_node <- function(tree) {
    .Call(ffi_r_ts_tree_root_node, tree)
}
