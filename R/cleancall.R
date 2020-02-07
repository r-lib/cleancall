
call_with_cleanup <- function(ptr, ...) {
  .Call(ptr_call, pairlist(ptr, ...), parent.frame())
}
