#' @keywords internal
"_PACKAGE"

## usethis namespace: start
#' @import rlang
#' @importFrom R6 R6Class
#' @useDynLib trees, .registration = TRUE
## usethis namespace: end
NULL

# Singleton environment
the <- new_environment()
