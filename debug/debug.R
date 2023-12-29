devtools::clean_dll()
devtools::load_all()

lib_dir <- file.path(tempdir(), "rtreesitter")
lib <- file.path(lib_dir, "languages.so")

repos <- c(
  "r-lib/tree-sitter-r"
)

lib_dir <- file.path(lib_dir, basename(repos))

processx::run("git", c("clone", "--depth=1", sprintf("https://github.com/%s", repos), lib_dir))

lines <- readLines(file.path(lib_dir, "src", "parser.c"))
lines <- gsub("<tree_sitter/parser.h>", '"tree_sitter/parser.h"', lines)
writeLines(lines, file.path(lib_dir, "src", "parser.c"))

lines <- readLines(file.path(lib_dir, "src", "scanner.c"))
lines <- gsub("<tree_sitter/parser.h>", '"tree_sitter/parser.h"', lines)
writeLines(lines, file.path(lib_dir, "src", "scanner.c"))

lib_src <- file.path(lib_dir, 'src')
makevars_content <- glue::glue("PKG_CPPFLAGS=-I{lib_src}")
brio::write_lines(makevars_content, file.path(lib_src, "Makevars"))

out <- language_build(lib_dir, lib)

is.loaded("tree_sitter_r", PACKAGE = "languages")
is.loaded("ts_symbol_metadata")

language <- language_get("tree_sitter_r")

language_node_id_for_kind(language, "parameters", TRUE)
