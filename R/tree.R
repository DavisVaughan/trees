Tree <- R6::R6Class(
    "Tree",
    cloneable = FALSE,
    private = list(
        .pointer = NULL,
        .language = NULL,
        .edit_count = 0L
    ),
    public = list(
        initialize = function(pointer, language) {
            tree_initialize(self, private, pointer, language)
        },

        language = function() {
            tree_language(self, private)
        },

        root_node = function() {
            tree_root_node(self, private)
        },

        edit_count = function() {
            tree_edit_count(self, private)
        }
    )
)

tree_initialize <- function(self, private, pointer, language) {
    private$.pointer <- pointer
    private$.language <- language
    self
}

tree_language <- function(self, private) {
    private$.language
}

tree_root_node <- function(self, private) {
    pointer <- private$.pointer
    raw <- .Call(ffi_r_ts_tree_root_node, pointer)
    new_node(raw, self)
}

tree_edit_count <- function(self, private) {
    private$.edit_count
}
