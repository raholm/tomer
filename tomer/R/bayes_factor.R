#' @export
log_bayes_factor <- function(topic_indicators, ntopics, beta) {
    .check_input_bayes_factor(topic_indicators, ntopics, beta)

    ## -1 to make it compatible with C++
    compute_log_bayes_factor_cpp(topic_indicators, ntopics, beta)
}

.check_input_bayes_factor <- function(topic_indicators, ntopics, beta) {
    checkr::assert_integer(topic_indicators, lower=0, upper=ntopics - 1)
    checkr::assert_integer(ntopics, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0, upper=1)
}
