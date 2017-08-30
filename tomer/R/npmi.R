#' @export
npmi <- function(topics, documents, window_size) {
    stopifnot(is.list(topics) & is.list(documents))
    stopifnot(sum(sapply(topics, is.character)) == length(topics))
    stopifnot(sum(sapply(documents, is.character)) == length(documents))
    checkr::assert_integer(window_size, len=1, lower=0)

    evaluate_npmi_cpp(topics, documents, window_size)
}
