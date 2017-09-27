#' @export
npmi <- function(topics, documents, window_size, cache=FALSE, cache_dst=NULL) {
    stopifnot(is.character(topics) & is.character(documents))
    checkr::assert_integer(window_size, len=1, lower=0)
    checkr::assert_logical(cache, len=1)

    if (cache) {
        checkr::assert_string(cache_dst)
        evaluate_npmi_with_cache_cpp(topics, documents, window_size, cache_dst)
    } else {
        evaluate_npmi_cpp(topics, documents, window_size)
    }
}
