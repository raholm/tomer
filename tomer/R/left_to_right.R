#' @title Left-to-right evaluation algorithm
#'
#' @description This is an algorithm for approximating p(w | ...) blabla
#'
#' @export
evaluate_left_to_right <- function(test_documents, model, ntopics,
                                   alpha, beta, nparticles, resampling) {
    checkr::assert_tidy_table(model, c("type", "token", "topic"))
    checkr::assert_tidy_table(test_documents, c("id", "token"))
    checkr::assert_integer(ntopics, len=1, lower=1)
    checkr::assert_integer(nparticles, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0)
    checkr::assert_numeric(alpha, len=ntopics, lower=0)
    checkr::assert_logical(resampling, len=1)

    ndocs <- length(unique(test_documents$id))

    ## The following -1s are there to make it
    ## compatible with C++ (Zero-based numbering
    ## https://en.wikipedia.org/wiki/Zero-based_numbering)

    alphabet <- model %>%
        dplyr::group_by(type, token) %>%
        dplyr::filter(row_number() == 1) %>%
        dplyr::ungroup() %>%
        dplyr::select(type, token) %>%
        dplyr::mutate(type=as.numeric(type) - 1,
                      token=as.character(token))

    topic_counts <- model %>%
        dplyr::group_by(topic) %>%
        dplyr::summarise(count=n()) %>%
        dplyr::ungroup() %>%
        dplyr::mutate(topic=as.numeric(topic) - 1)

    type_topic_counts <- model %>%
        dplyr::group_by(type, topic) %>%
        dplyr::summarise(count=n()) %>%
        dplyr::ungroup() %>%
        dplyr::mutate(type=as.numeric(type) - 1,
                      topic=as.numeric(topic) - 1)

    evaluate_left_to_right_cpp(test_documents,
                               ndocs,
                               alphabet,
                               ntopics,
                               topic_counts,
                               type_topic_counts,
                               alpha,
                               beta,
                               nparticles,
                               resampling)
}
