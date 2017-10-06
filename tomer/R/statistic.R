#' @export
markovian_bf_test <- function(topic_indicators, ntopics, beta) {
    .check_input_markovian_bf_test(topic_indicators, ntopics, beta)
    compute_markovian_bf_test_cpp(topic_indicators, ntopics, beta)
}

.check_input_markovian_bf_test <- function(topic_indicators, ntopics, beta) {
    checkr::assert_integer(topic_indicators, lower=0, upper=ntopics - 1)
    checkr::assert_integer(ntopics, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0)
}

#' @export
markovian_lr_test <- function(topic_indicators, ntopics) {
    .check_input_markovian_lr_test(topic_indicators, ntopics)
    compute_markovian_lr_test_cpp(topic_indicators, ntopics)
}

.check_input_markovian_lr_test <- function(topic_indicators, ntopics) {
    checkr::assert_integer(topic_indicators, lower=0, upper=ntopics - 1)
    checkr::assert_integer(ntopics, len=1, lower=1)
}

#' @export
chunking_bf_test <- function(topic_indicators, ntopics, beta) {
    .check_input_chunking_bf_test(topic_indicators, ntopics, beta)
    compute_chunking_bf_test_cpp(topic_indicators, ntopics, beta)
}

.check_input_chunking_bf_test <- function(topic_indicators, ntopics, beta) {
    lapply(topic_indicators, function(tis) {
        checkr::assert_integer(tis, lower=0, upper=ntopics - 1)
    })
    checkr::assert_integer(ntopics, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0)
}
