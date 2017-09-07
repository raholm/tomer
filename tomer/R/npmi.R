#' @export
npmi <- function(topics, documents, window_size) {
    stopifnot(is.list(topics) & is.list(documents))
    stopifnot(all(sapply(topics, is.character)))
    stopifnot(all(sapply(documents, is.character)))
    checkr::assert_integer(window_size, len=1, lower=0)

    evaluate_npmi_cpp(topics, documents, window_size)
}