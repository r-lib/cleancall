
test_no_context <- function() {
  .Call(ptr_return, pairlist(NULL))
}
test_return <- function(node) {
  ptr_invoke(ptr_return, node)
}
test_jump <- function(node) {
  ptr_invoke(ptr_jump, node)
}
test_jumpy_cb <- function(node) {
  ptr_invoke(ptr_jumpy_cb, node)
}
test_no_cb <- function() {
  ptr_invoke(ptr_no_cb)
}
