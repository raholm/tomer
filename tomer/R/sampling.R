#' @export
sample_document <- function(size, model, alpha) {
    checkr::assert_integer(size, lower=1)

    n <- length(size)
    ntopics <- nrow(model)
    nwords <- ncol(model)

    if (length(alpha) != 1 & length(alpha) != ntopics)
        stop("length of alpha has to be either one or the number of topics.")

    if (length(alpha) == 1)
        alpha <- rep(alpha, ntopics)

    topic_probs <- MCMCpack::rdirichlet(n, alpha)

    topic_types <- 0:(ntopics - 1)
    token_types <- 0:(nwords - 1)

    sampler <- function(id) {
        topic_indicators <- sample(x=topic_types,
                                   size=size[id],
                                   replace=TRUE,
                                   prob=topic_probs[id,])

        tokens <- sapply(topic_indicators, function(topic) {
            sample(x=token_types,
                   size=1,
                   replace=TRUE,
                   prob=model[topic + 1,])

        })

        dplyr::data_frame(doc=rep(id, length(tokens)),
                          token=tokens,
                          topic=topic_indicators)
    }

    plyr::rbind.fill(lapply(1:n, sampler)) %>%
        dplyr::as_data_frame()
}

#' @export
sample_topic_indicators <- function(size, model, alpha) {
    checkr::assert_integer(size, lower=1)

    n <- length(size)
    ntopics <- nrow(model)
    nwords <- ncol(model)

    if (length(alpha) != 1 & length(alpha) != ntopics)
        stop("length of alpha has to be either one or the number of topics.")

    if (length(alpha) == 1)
        alpha <- rep(alpha, ntopics)

    topic_probs <- MCMCpack::rdirichlet(n, alpha)

    topic_types <- 0:(ntopics - 1)
    token_types <- 0:(nwords - 1)

    sampler <- function(id) {
        topic_indicators <- sample(x=topic_types,
                                   size=size[id],
                                   replace=TRUE,
                                   prob=topic_probs[id,])

        dplyr::data_frame(doc=rep(id, length(tokens)),
                          topic=topic_indicators)
    }

    plyr::rbind.fill(lapply(1:n, sampler)) %>%
        dplyr::as_data_frame()
}
