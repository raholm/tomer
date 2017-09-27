#' @export
topic_coherence <- function(topics, documents, cache=FALSE, cache_dst=NULL) {
    stopifnot(is.character(topics) & is.character(documents))

    if (cache) {
        checkr::assert_string(cache_dst)
        evaluate_topic_coherence_with_cache_cpp(topics, documents, window_size, cache_dst)
    } else {
        evaluate_topic_coherence_cpp(topics, documents, window_size)
    }
}
