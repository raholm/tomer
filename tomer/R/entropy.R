#' Calculates the entropy
#'
#' @param probs Probabilities to calculate entropy of
#' @param freq If set to \code{TRUE}, then \code{probs} are interpreted as frequencies. Defaults to FALSE.
#' @param base The base with respect to which logarithms are computed. Defaults to base 2="2". Possible options are "2", "10", and "nat".
entropy <- function(probs, freq=FALSE, base="2") {
    ## assert_numeric
    checkr::assert_type(probs, "double")
    checkr::assert_character(base)
    checkr::assert_subset(base, c("2", "10", "nat"))

    if (freq) {
        probs <- probs / sum(probs)
    }

    log_func <- switch(base,
                       nat=log,
                       "2"=log2,
                       "10"=log10)

    sum(probs * log_func(probs))
}
