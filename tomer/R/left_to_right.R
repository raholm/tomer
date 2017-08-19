#' @title Left-to-right evaluation algorithm
#'
#' @description This is an algorithm for approximating p(w | ...) blabla
#'
#' @export
evaluate_left_to_right <- function(test_documents, model, ntopics,
                                   alpha, beta, nparticles, resampling) {
    checkr::assert_tidy_table(model, c("type", "token", "topic"))
    checkr::assert_integer(model$type)
    checkr::assert_integer(model$topic)
    checkr::assert_factor(model$token)

    checkr::assert_tidy_table(test_documents, c("id", "token"))
    checkr::assert_factor(test_documents$id)
    checkr::assert_factor(test_documents$token)

    checkr::assert_integer(ntopics, len=1, lower=1)
    checkr::assert_integer(nparticles, len=1, lower=1)
    checkr::assert_numeric(beta, len=1, lower=0)
    checkr::assert_numeric(alpha, len=ntopics, lower=0)
    checkr::assert_logical(resampling, len=1)

    ndocs <- length(unique(test_documents$id))

    alphabet <- model %>%
        dplyr::group_by(type, token) %>%
        dplyr::filter(row_number() == 1) %>%
        dplyr::ungroup() %>%
        dplyr::select(type, token) %>%
        dplyr::mutate(token=as.character(token))

    topic_counts <- model %>%
        dplyr::group_by(topic) %>%
        dplyr::summarise(count=n()) %>%
        dplyr::ungroup()

    type_topic_counts <- model %>%
        dplyr::group_by(type, topic) %>%
        dplyr::summarise(count=n()) %>%
        dplyr::ungroup()

    test_documents <- test_documents %>%
        mutate(id=as.numeric(id) - 1, ## has to do -1 to be compatible with C++ (zero-index)
               token=as.character(token))

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
