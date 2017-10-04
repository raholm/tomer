#' @export
topic_coherence <- function(topics, documents, cache=FALSE, cache_dst=NULL) {
    stopifnot(is.character(topics))

    if (!cache || (cache && !file.exists(cache_dst)))
        stopifnot(is.character(documents))

    if (cache) {
        checkr::assert_string(cache_dst)

        if (is.null(documents))
            documents <- character(length=0)

        evaluate_topic_coherence_with_cache_cpp(topics, documents, cache_dst)
    } else {
        evaluate_topic_coherence_cpp(topics, documents)
    }
}

#' @export
topic_coherence_create_cache <- function(documents, dst) {
    stopifnot(!file.exists(dst))
    stopifnot(is.character(documents))
    create_word_count_cache_cpp(documents, 0, dst)
}


#' @export
topic_coherence_file <- function(topics, filename) {
    evaluate_topic_coherence_from_file_cpp(topics, filename)
}


test_topic_coherence_file <- function() {
    topics <- read.table("/home/textanalysis/git/topic_corpus_curation/experiments/sotu/topic/rare_word_removal/topics_after_0.0001_100_0.1_0.01_50_100.txt", fill=TRUE, stringsAsFactors=FALSE)
    topics <- sapply(as.list(topics), paste, collapse=" ")
    corpus_filename <- "/home/textanalysis/data/corpus/nyt_complete.txt"

    timer <- Sys.time()
    topic_coherence_file(topics, corpus_filename)
    Sys.time() - timer
}
