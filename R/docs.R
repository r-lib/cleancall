#' @useDynLib cleancall, .registration = TRUE
"_PACKAGE"

#' Call a native routine within an exit context
#'
#' C functions called this way can call the `r_call_on_exit()` and/or
#' `r_call_on_early_exit()` functions to establish exit handlers.
#'
#' @param ptr A native pointer object.
#' @param ... Arguments for the native routine.
#'
#' Handlers installed via `r_call_on_exit()` are always executed on exit.
#' Handlers installed via `r_call_on_early_exit()` are only executed
#' on early exit, i.e. _not_ on normal termination.
#'
#' @section C API:
#'
#' * `void r_call_on_exit(void (*fn)(void* data), void *data)`
#'
#'     Push an exit handler to the stack. This exit handler is always
#'     executed, i.e. both on normal and early exits.
#'
#'     Exit handlers are executed right after the function called from
#'     `call_with_cleanup()` exits. (Or the function used in
#'     `r_with_cleanup_context()`, if the cleanup context was established
#'     from C.)
#'
#'     Exit handlers are executed in reverse order (last in is first out,
#'     LIFO). Exit handlers pushed with `r_call_on_exit()` and
#'     `r_call_on_early_exit()` share the same stack.
#'
#'     Best practice is to use this function immediately after acquiring a
#'     resource, with the appropriate cleanup function for that resource.
#'
#' * `void r_call_on_early_exit(void (*fn)(void* data), void *data)`
#'
#'     Push an exit handler to the stack. This exit handler is only
#'     executed on early exists, _not_ on normal termination.
#'
#'     Exit handlers are executed right after the function called from
#'     `call_with_cleanup()` exits. (Or the function used in
#'     `r_with_cleanup_context()`, if the cleanup context was established
#'     from C.)
#'
#'     Exit handlers are executed in reverse order (last in is first out,
#'     LIFO). Exit handlers pushed with `r_call_on_exit()` and
#'     `r_call_on_early_exit()` share the same stack.
#'
#'     Best practice is to use this function immediately after acquiring a
#'     resource, with the appropriate cleanup function for that resource.
#'
#' * `SEXP r_with_cleanup_context(SEXP (*fn)(void* data), void* data)`
#'
#'     Establish a cleanup stack and call `fn` with `data`. This function
#'     can be used to establish a cleanup stack from C code.
#'
#' @usage
#' call_with_cleanup(ptr, ...)
#' @export
#' @seealso The package README file.
#' @name call_with_cleanup
NULL
