#' @export
topic_coherence <- function(topics, documents) {
    stopifnot(is.character(topics) & is.character(documents))

    evaluate_topic_coherence_cpp(topics, documents)
}
