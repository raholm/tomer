#' Calculates the entropy
#'
#' @param probs Probabilities to calculate entropy of
#' @param freq If set to \code{TRUE}, then \code{probs} are interpreted as frequencies. Defaults to FALSE.
#' @param base The base with respect to which logarithms are computed. Defaults to base 2="shannon". Possible options are "shannon" for base 2, "nat" for base e, and "hartley" for base 10.
#'
#' @export
entropy <- function(probs, freq=FALSE, base="shannon") {
    checkr::assert_type(probs, "double")
    checkr::assert_character(base)
    checkr::assert_choice(base, c("shannon", "nat", "hartley"))

    if (freq) {
        probs <- probs / sum(probs)
    }

    log_func <- switch(base,
                       shannon=log2,
                       nat=log,
                       hartley=log10)

    -sum(probs * log_func(probs))
}
