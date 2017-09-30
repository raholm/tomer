#' @export
topic_coherence <- function(topics, documents, cache=FALSE, cache_dst=NULL) {
    stopifnot(is.character(topics))

    if (!cache || (cache && !file.exists(cache_dst)))
        stopifnot(is.character(documents))

    if (cache) {
        checkr::assert_string(cache_dst)

        if (is.null(documents))
            documents <- character(length=0)

        evaluate_topic_coherence_with_cache_cpp(topics, documents, cache_dst)
    } else {
        evaluate_topic_coherence_cpp(topics, documents)
    }
}

#' @export
topic_coherence_create_cache <- function(documents, dst) {
    stopifnot(!file.exists(dst))
    stopifnot(is.character(documents))
    create_word_count_cache_cpp(documents, 0, dst)
}
