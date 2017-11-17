#' @title Left-to-right evaluation algorithm
#'
#' @description This is an algorithm for approximating p(w | ...) blabla
#'
#' @export
evaluate_left_to_right <- function(test_documents, model, ntopics,
                                   alpha, beta, nparticles, resampling, seed=0) {
    checkmate::assert_class(model, "data.frame")
    checkmate::assert_subset(c("type", "token", "topic"), names(model))
    checkmate::assert_numeric(ntopics, len=1, lower=1)
    checkmate::assert_numreic(nparticles, len=1, lower=1)
    checkmate::assert_numeric(beta, len=1, lower=0)
    checkmate::assert_numeric(alpha, len=ntopics, lower=0)
    checkmate::assert_logical(resampling, len=1)
    checkmate::assert_numeric(seed, len=1)

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
                               resampling,
                               seed)
}
