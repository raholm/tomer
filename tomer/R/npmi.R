#' @export
npmi <- function(topics, documents, window_size, cache=FALSE, cache_dst=NULL) {
    stopifnot(is.character(topics))
    checkr::assert_integer(window_size, len=1, lower=0)
    checkr::assert_logical(cache, len=1)

    if (!cache || (cache && !file.exists(cache_dst)))
        stopifnot(is.character(documents))

    if (cache) {
        checkr::assert_string(cache_dst)

        if (is.null(documents))
            documents <- character(length=0)

        evaluate_npmi_with_cache_cpp(topics, documents, window_size, cache_dst)
    } else {
        evaluate_npmi_cpp(topics, documents, window_size)
    }
}
