#' @title Left-to-right evaluation algorithm
#'
#' @description This is an algorithm for approximating p(w | ...) blabla
#'
#' @export
evaluate_left_to_right <- function(corpus, state, n_topics, beta, alpha, n_particles) {
    checkr::assert_tidy_table(state, c("type", "token", "topic"))
    checkr::assert_tidy_table(corpus, c("id", "text"))
    checkr::assert_integer(n_topics, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0)
    checkr::assert_numeric(alpha, len=n_topics, lower=0)

    n_docs <- nrow(corpus)

    tokens <- corpus %>%
        texcur::tf_tokenize()  %>%
        dplyr::mutate(id=as.numeric(id))

    alphabet <- state %>%
        dplyr::group_by(type, token) %>%
        dplyr::filter(row_number() == 1) %>%
        dplyr::ungroup() %>%
        dplyr::select(type, token) %>%
        dplyr::mutate(type=as.numeric(type) - 1,
                      token=as.character(token))

    topic_counts <- state %>%
        dplyr::group_by(topic) %>%
        dplyr::summarise(count=n()) %>%
        dplyr::ungroup() %>%
        dplyr::mutate(topic=as.numeric(topic) - 1)

    type_topic_counts <- state %>%
        dplyr::group_by(type, topic) %>%
        dplyr::summarise(count=n()) %>%
        dplyr::ungroup() %>%
        dplyr::mutate(type=as.numeric(type) - 1,
                      topic=as.numeric(topic) - 1)

    evaluate_left_to_right_cpp(tokens, n_docs, alphabet, topic_counts,
                               type_topic_counts, n_topics, beta, alpha,
                               n_particles);
}
