new_parser <- function() {
    .Call(ffi_r_ts_new_parser)
}

parser_get_language <- function(parser) {
    .Call(ffi_r_ts_parser_get_language, parser)
}

parser_set_language <- function(parser, language) {
    .Call(ffi_r_ts_parser_set_language, parser, language)
}

parser_parse <- function(parser, code) {
    .Call(ffi_r_ts_parser_parse, parser, code)
}
