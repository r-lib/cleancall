#' @useDynLib cleancall, .registration = TRUE
"_PACKAGE"

#' Call a native routine within an exit context
#'
#' @param ptr A native pointer object.
#' @param ... Arguments for the native routine.
#'
#' @export
ptr_invoke <- function(ptr, ...) {
  .Call(cleancall_invoke, pairlist(ptr, ...), parent.frame())
}
