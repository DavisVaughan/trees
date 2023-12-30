Parser <- R6::R6Class(
    "Parser",
    cloneable = FALSE,
    private = list(
        .pointer = NULL,
        .language = NULL
    ),
    public = list(
        initialize = function(language) {
            parser_initialize(self, private, language)
        },

        language = function() {
            parser_language(self, private)
        },

        parse = function(text) {
            parser_parse(self, private, text)
        }
    )
)

parser_initialize <- function(self, private, language) {
    private$.language <- language
    language <- language$pointer()

    pointer <- .Call(ffi_r_ts_parser_initialize, language)
    private$.pointer <- pointer

    self
}

parser_language <- function(self, private) {
    private$.language
}

parser_parse <- function(self, private, text) {
    pointer <- private$.pointer
    language <- private$.language

    text <- enc2utf8(text)

    pointer <- .Call(ffi_r_ts_parser_parse, pointer, text)

    Tree$new(pointer, language)
}
