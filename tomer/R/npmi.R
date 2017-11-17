#' @export
npmi <- function(topics, documents, window_size, cache=FALSE, cache_dst=NULL) {
    stopifnot(is.character(topics))
    checkmate::assert_numeric(window_size, len=1, lower=0)
    checkmate::assert_logical(cache, len=1)

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

#' @export
npmi_create_cache <- function(documents, window_size, dst) {
    stopifnot(!file.exists(dst))
    stopifnot(is.character(documents))
    checkmate::assert_integer(window_size, len=1, lower=0)
    create_word_count_cache_cpp(documents, window_size, dst)
}

#' @export
npmi_file <- function(topics, window_size, filename) {
    evaluate_npmi_from_file_cpp(topics, window_size, filename)
}
