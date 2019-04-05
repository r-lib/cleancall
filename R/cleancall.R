#' @useDynLib cleancall, .registration = TRUE
"_PACKAGE"

#' Call a native routine within an exit context
#'
#' @param ptr A native pointer object.
#' @param ... Arguments for the native routine.
#'
#' @export
call_with_cleanup <- function(ptr, ...) {
  .Call(cleancall_call, pairlist(ptr, ...), parent.frame())
}
