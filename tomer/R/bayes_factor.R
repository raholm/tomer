log_bayes_factor <- function(corpus, mode, ntopics, beta) {
    .check_input_bayes_factor(corpus, mode, ntopics, beta)

    topic_indicators <- types_to_topic_indicators(corpus$token, mode)
    module <- get_bayes_factor_module()
    module$compute_log_bayes_factor(topic_indicators, n_topics, beta)
}

.check_input_bayes_factor <- function(corpus, mode, ntopics, beta) {
    checkr::assert_tidy_table(corpus, "token")
    checkr::assert_integer(ntopics, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0, upper=1)
}
