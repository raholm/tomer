log_bayes_factor <- function(corpus, n_topics, beta) {
    .check_input_bayes_factor(corpus, n_topics, beta)

    module <- get_module("mod_bayes_factor")
    module$compute_log_bayes_factor(corpus$topic, n_topics, beta)
}

.check_input_bayes_factor <- function(corpus, n_topics, beta) {
    checkr::assert_tidy_table(corpus, "topic")
    checkr::assert_integer(n_topics, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0, upper=1)
}
