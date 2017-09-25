#' @export
npmi <- function(topics, documents, window_size) {
    stopifnot(is.character(topics) & is.character(documents))
    checkr::assert_integer(window_size, len=1, lower=0)

    evaluate_npmi_cpp(topics, documents, window_size)
}
