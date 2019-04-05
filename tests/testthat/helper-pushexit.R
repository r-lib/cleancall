
test_no_context <- function() {
  .Call(ptr_test_return, pairlist(NULL))
}
test_return <- function(node) {
  call_with_cleanup(ptr_test_return, node)
}
test_jump <- function(node) {
  call_with_cleanup(ptr_test_jump, node)
}
test_jumpy_cb <- function(node) {
  call_with_cleanup(ptr_test_jumpy_cb, node)
}
test_no_cb <- function() {
  call_with_cleanup(ptr_test_no_cb)
}
