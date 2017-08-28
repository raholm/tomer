context("C++")

test_that("Catch unit tests pass", {
    expect_cpp_tests_pass("testthat")
    expect_true(run_testthat_tests())
})
