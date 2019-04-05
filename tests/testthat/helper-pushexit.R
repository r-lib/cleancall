
test_no_context <- function() {
  .Call(ptr_return, pairlist(NULL))
}
test_return <- function(node) {
  call_with_cleanup(ptr_return, node)
}
test_jump <- function(node) {
  call_with_cleanup(ptr_jump, node)
}
test_jumpy_cb <- function(node) {
  call_with_cleanup(ptr_jumpy_cb, node)
}
test_no_cb <- function() {
  call_with_cleanup(ptr_no_cb)
}
