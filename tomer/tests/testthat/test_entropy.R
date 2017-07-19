test_that("entropy throws error for invalid input", {
    expect_error(entropy(c("a", "b", "c")))
    expect_error(entropy(c(1, 2, 3), base="1"))
    expect_error(entropy(c(1, 2, 3), base=2))
    expect_error(entropy(c(1, 2, 3), base=10))
})

test_that("entropy does no throw error for valid input", {
    expect_message(entropy(c(1, 2, 3), base="2"), NA)
    expect_message(entropy(c(1, 2, 3), base="10"), NA)
    expect_message(entropy(c(1, 2, 3), base="nat"), NA)
})

test_that("entropy base '2' calculates correctly given probabilities", {

})

test_that("entropy base '2' calculates correctly given frequencies", {

})

test_that("entropy base '10' calculates correctly given probabilities", {

})

test_that("entropy base '10' calculates correctly given frequencies", {

})

test_that("entropy base 'nat' calculates correctly given probabilities", {

})

test_that("entropy base 'nat' calculates correctly given frequencies", {

})
