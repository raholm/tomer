#' @export
topic_coherence <- function(topics, documents) {
    stopifnot(is.list(topics) & is.list(documents))
    stopifnot(all(sapply(topics, is.character)))
    stopifnot(all(sapply(documents, is.character)))

    evaluate_topic_coherence_cpp(topics, documents)
}
