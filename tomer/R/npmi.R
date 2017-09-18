#' @export
npmi <- function(topics, documents, window_size) {
    stopifnot(is.character(topics) & is.character(documents))
    checkr::assert_integer(window_size, len=1, lower=0)

    evaluate_npmi_cpp(topics, documents, window_size)
}

## #' @export
## npmi2 <- function(topics, documents, window_size) {
##     stopifnot(is.list(topics) & is.character(documents))
##     stopifnot(all(sapply(topics, is.character)))
##     checkr::assert_integer(window_size, len=1, lower=0)

##     evaluate_npmi2_cpp(topics, documents, window_size)
## }

## #' @export
## npmi3 <- function(topics, documents, window_size) {
##     stopifnot(is.character(topics) & is.character(documents))
##     checkr::assert_integer(window_size, len=1, lower=0)

##     evaluate_npmi3_cpp(topics, documents, window_size)
## }

## npmi_test <- function() {
##     corpus <- get_corpus_nyt()$text
##     object.size(corpus) / (1024^2)
##     window_size <- 20
##     print("Running npmi")
##     npmi3(c("a b", "a b"), corpus, window_size)
##     ## npmi2(list(c("a", "b"), c("a", "b")), corpus, window_size)
## }
