context("Entropy")

test_that("entropy throws error for invalid input", {
    expect_error(entropy(c("a", "b", "c")))
    expect_error(entropy(c(1, 2, 3), base=2))
    expect_error(entropy(c(1, 2, 3), base=10))
    expect_error(entropy(c(1, 2, 3), base="2"))
    expect_error(entropy(c(1, 2, 3), base="e"))
    expect_error(entropy(c(1, 2, 3), base="10"))
})

test_that("entropy does no throw error for valid input", {
    expect_message(entropy(c(1, 2, 3), base="shannon"), NA)
    expect_message(entropy(c(1, 2, 3), base="hartley"), NA)
    expect_message(entropy(c(1, 2, 3), base="nat"), NA)
})

test_that("entropy base 'shannon' calculates correctly given probabilities", {
    probs <- c(0.5, 0.5)
    actual <- entropy(probs, base="shannon")
    expected <- 1
    expect_equal(actual, expected)
})

test_that("entropy base 'shannon' calculates correctly given frequencies", {
    probs <- c(1, 1)
    actual <- entropy(probs, freq=TRUE, base="shannon")
    expected <- 1
    expect_equal(actual, expected)
})

test_that("entropy base 'hartley' calculates correctly given probabilities", {

})

test_that("entropy base 'hartley' calculates correctly given frequencies", {

})

test_that("entropy base 'nat' calculates correctly given probabilities", {

})

test_that("entropy base 'nat' calculates correctly given frequencies", {

})
