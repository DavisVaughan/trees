on_load({
    the$languages <- new_environment()
})

Language <- R6::R6Class(
    "Language",
    cloneable = FALSE,
    private = list(
        .name = NULL,
        .pointer = NULL,
        .dllinfo = NULL
    ),
    public = list(
        initialize = function(name, ..., dllinfo = NULL) {
            check_dots_empty0(...)
            language_initialize(self, private, name, dllinfo)
        },

        unload = function() {
            language_unload(self, private)
        },

        name = function() {
            language_name(self, private)
        },

        version = function() {
            language_version(self, private)
        },

        id_for_node_kind = function(kind, ..., named = FALSE) {
            check_dots_empty0(...)
            language_id_for_node_kind(self, private, kind, named)
        },

        node_kind_for_id = function(id) {
            language_node_kind_for_id(self, private, id)
        }
    )
)

language_initialize <- function(self, private, name, dllinfo) {
    private$.name <- name

    if (identical(name, "r_embedded")) {
        private$.pointer <- language_get_pointer_r_embedded()
    } else {
        fn <- paste0("tree_sitter_", name)
        private$.pointer <- language_get_pointer(fn)
    }

    private$.dllinfo <- dllinfo

    self
}

language_unload <- function(self, private) {
    if (is.null(private$.pointer)) {
        abort("Language has already been unloaded.")
    }
    if (language_is_r_embedded(self, private)) {
        abort("Can't unload the embedded R language.")
    }

    name <- language_name(self, private)
    inform_unloading(name)

    dllinfo <- private$.dllinfo
    shared_library <- unclass(dllinfo)$path

    dyn.unload(shared_library)

    private$.dllinfo <- NULL
    private$.pointer <- NULL

    invisible(NULL)
}

inform_unloading <- function(name) {
    message <- cli::format_inline(c(
        "Unloading existing `{name}` language. ",
        "Any existing parsers, trees, or nodes that reference this language are now invalid!"
    ))
    rlang::inform(message)
}

language_is_r_embedded <- function(self, private) {
    identical(private$.name, "r_embedded")
}

language_id_for_node_kind <- function(self, private, kind, named) {
    pointer <- private$.pointer
    .Call(ffi_r_ts_language_id_for_node_kind, pointer, kind, named)
}

language_node_kind_for_id <- function(self, private, id) {
    pointer <- private$.pointer
    .Call(ffi_r_ts_language_node_kind_for_id, pointer, id)
}

language_name <- function(self, private) {
    private$.name
}

language_version <- function(self, private) {
    pointer <- private$.pointer
    .Call(ffi_r_ts_language_version, pointer)
}

language_get_pointer_r_embedded <- function() {
    .Call(ffi_r_ts_language_get_pointer_r_embedded)
}

language_get_pointer <- function(name) {
    .Call(ffi_r_ts_language_get_pointer, name)
}

# ------------------------------------------------------------------------------

language_r <- function() {
    language <- env_get(the$languages, "r_embedded", default = NULL)

    if (is.null(language)) {
        language <- Language$new("r_embedded")
        env_poke(the$languages, "r_embedded", language)
    }

    language
}

language_from_url <- function(url) {
    dir <- dir_temp()

    tar <- tempfile("tar", tmpdir = dir, fileext = ".tar.gz")
    untar <- tempfile("untar", tmpdir = dir)

    # Download into `trees/tar`
    utils::download.file(url = url, destfile = tar)

    # Unpack into `trees/untar`
    untar(tar, exdir = untar)

    # Will be named like `ref-branch/`
    path <- dir(path = untar, full.names = TRUE)

    stopifnot(length(path) == 1L, is.character(path))

    language_from_path(path)
}

# https://docs.github.com/en/repositories/working-with-files/using-files/downloading-source-code-archives#source-code-archive-urls
# https://github.com/tree-sitter/tree-sitter-python/archive/refs/heads/master.tar.gz
language_from_github <- function(ref, ..., branch = "main") {
    check_dots_empty0(...)
    url <- sprintf("https://github.com/%s/archive/refs/heads/%s.tar.gz", ref, branch)
    language_from_url(url)
}

language_from_path <- function(path) {
    dir <- dir_temp()

    if (is_windows()) {
        ext <- ".dll"
    } else {
        ext <- ".so"
    }

    # The shared object / DLL file everything ends up in
    output <- tempfile("language", tmpdir = dir, fileext = ext)

    # All source files should be in a `src/` directory
    src <- file.path(path, "src")
    src <- normalizePath(src, mustWork = TRUE)

    # Look up language name in the parser file
    parser <- file.path(src, "parser.c")
    parser <- normalizePath(parser, mustWork = TRUE)
    name <- language_name_from_parser_c(parser)

    # Unload existing language if one existed
    if (!is.null(the$languages[[name]])) {
        language <- the$languages[[name]]
        the$languages[[name]] <- NULL
        language$unload()
    }

    # `parser.c` and optionally `scanner.cc`.
    # Typically also a `tree_sitter/parser.h` which should be found automatically.
    files <- list.files(path = src, pattern = "(parser|scanner)[.]cc?", full.names = TRUE)

    # Compiled shared library and then dynamically load it in
    callr::rcmd("SHLIB", c("--output", output, files), fail_on_status = TRUE)

    dllinfo <- dyn.load(output)

    language <- Language$new(name = name, dllinfo = dllinfo)
    the$languages[[name]] <- language

    language
}

language_name_from_parser_c <- function(x) {
    lines <- readLines(x)

    pattern <- "extern const TSLanguage \\*tree_sitter_(?<name>[[:alnum:]_]+)\\(void\\) \\{"

    loc <- which(grepl(pattern, lines, perl = TRUE))

    if (length(loc) != 1L) {
        abort("`parser.c` file must contain a function signature like `extern const TSLanguage *tree_sitter_<language>(void) {`.")
    }

    line <- lines[[loc]]

    match <- regexec(pattern, line, perl = TRUE)
    match <- regmatches(line, match)
    name <- match[[1L]][["name"]]

    if (!is_string(name)) {
        abort("Failed to extract name from `parser.c`.", .internal = TRUE)
    }

    name
}

is_windows <- function() {
    identical(tolower(Sys.info()[["sysname"]]), "windows")
}

dir_temp <- function() {
    dir <- tempdir()

    # All our files go in `trees/`
    dir <- tempfile("trees", tmpdir = dir)

    if (!dir.exists(dir)) {
        dir.create(dir)
    }

    dir
}
