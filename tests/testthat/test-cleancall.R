context("cleancall")

exp <- pairlist(NULL, quote(first), quote(second))

test_that("handlers are called on return", {
  node <- pairlist(NULL)
  test_return(node)
  expect_identical(node, exp)
})

test_that("handlers are called on error", {
  node <- pairlist(NULL)
  expect_error(test_jump(node), "tilt")
  expect_identical(node, exp)
})

test_that("handlers can jump", {
  node <- pairlist(NULL)
  expect_error(test_jumpy_cb(node), "jump")
  expect_identical(node, exp)
})

test_that("can call without callbacks", {
  expect_error(regexp = NA, test_no_cb())
})

test_that("pushing exit outside of context is an error", {
  expect_error(test_no_context(), "outside of an exit context")
})
