
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
  err <- tryCatch(test_jumpy_cb(node), error = function(e) e)
  expect_s3_class(err, "error")
  expect_match(conditionMessage(err), "jump")
  expect_identical(node, exp)
})

test_that("can call without callbacks", {
  expect_error(regexp = NA, test_no_cb())
})

test_that("pushing exit outside of context is an error", {
  expect_error(test_no_context(), "outside of an exit context")
})

test_that("early handlers ignored on success", {
  node <- pairlist(NULL)
  expect_null(test_early_ok(node))
  expect_identical(node, pairlist(NULL))
})

test_that("early handlers run on jump", {
  node <- pairlist(NULL)
  expect_error(test_early_jump(node), "jump")
  expect_identical(node, exp)
})

test_that("mixing early & other handlers", {
  node <- pairlist(NULL)
  expect_null(test_mixed(node))
  expect_identical(node, pairlist(NULL, quote(first)))
})
