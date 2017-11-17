#' @export
markovian_bf_test <- function(topic_indicators, ntopics, beta) {
    .check_input_markovian_bf_test(topic_indicators, ntopics, beta)
    compute_markovian_bf_test_cpp(topic_indicators, ntopics, beta)
}

.check_input_markovian_bf_test <- function(topic_indicators, ntopics, beta) {
    checkmate::assert_numeric(topic_indicators, lower=0, upper=ntopics - 1)
    checkmate::assert_numeric(ntopics, len=1, lower=1)
    checkmate::assert_numeric(beta, len=1, lower=0)
}

#' @export
markovian_lr_test <- function(topic_indicators, ntopics) {
    .check_input_markovian_lr_test(topic_indicators, ntopics)
    compute_markovian_lr_test_cpp(topic_indicators, ntopics)
}

.check_input_markovian_lr_test <- function(topic_indicators, ntopics) {
    checkmate::assert_numeric(topic_indicators, lower=0, upper=ntopics - 1)
    checkmate::assert_numeric(ntopics, len=1, lower=1)
}

#' @export
chunking_bf_test <- function(topic_indicators, ntopics, beta) {
    .check_input_chunking_bf_test(topic_indicators, ntopics, beta)
    compute_chunking_bf_test_cpp(topic_indicators, ntopics, beta)
}

.check_input_chunking_bf_test <- function(topic_indicators, ntopics, beta) {
    lapply(topic_indicators, function(tis) {
        checkmate::assert_numeric(tis, lower=0, upper=ntopics - 1)
    })
    checkmate::assert_numeric(ntopics, len=1, lower=1)
    checkmate::assert_numeric(beta, len=1, lower=0)
}
