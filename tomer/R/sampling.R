sample_document <- function(size, model, alpha) {
    ntopics <- nrow(model)
    nwords <- ncol(model)

    if (length(alpha) != 1 & length(alpha) != ntopics)
        stop("length of alpha has to be either one or the number of topics.")

    if (length(alpha) == 1)
        alpha <- rep(alpha, ntopics)

    topic_probs <- MCMCpack::rdirichlet(1, alpha)

    topic_indicators <- sample(x=1:ntopics,
                               size=size,
                               replace=TRUE,
                               prob=topic_probs)

    sample_word <- function(topic) {
        word_probs <- model[topic,]
        sample(x=1:nwords,
               size=1,
               replace=TRUE,
               prob=word_probs)
    }

    words <- sapply(topic_indicators, sample_word)

    dplyr::data_frame(token=words, topic=topic_indicators)
}

sample_corpus <- function(n, size, model, alpha) {
    ntopics <- nrow(model)
    nwords <- ncol(model)

    if (length(alpha) != 1 & length(alpha) != ntopics)
        stop("length of alpha has to be either one or the number of topics.")

    if (length(alpha) == 1)
        alpha <- rep(alpha, ntopics)

    if (length(size) != 1 & length(size) != n)
        stop("size must be equal length to 1 or n")

    if (length(size) == 1)
        size <- rep(size, n)

    topic_probs <- MCMCpack::rdirichlet(n, alpha)

    topic_types <- 1:ntopics
    token_types <- 1:nwords

    sample_document <- function(id) {
        topic_indicators <- sample(x=topic_types,
                                   size=size[id],
                                   replace=TRUE,
                                   prob=topic_probs[id,])

        tokens <- sapply(topic_indicators, function(topic) {
            sample(x=token_types,
                   size=1,
                   replace=TRUE,
                   prob=model[topic,])

        })

        dplyr::data_frame(id=rep(id, length(tokens)),
                          token=tokens,
                          topic=topic_indicators)
    }

    plyr::rbind.fill(lapply(1:n, sample_document)) %>%
        dplyr::as_data_frame()
}


model <- matrix(c(1, 2, 3, 3, 2, 1), nrow=2)
size <- c(2, 5, 10, 15)
alpha <- 5
n <- 4

document <- sample_document(size[3], model, alpha)
corpus <- sample_corpus(n, size, model, alpha)

words <- as.factor(c("foo", "bar", "foo1", "foo"))

corpus
corpus$token <- as.character(words[corpus$token])
corpus$id <- as.character(corpus$id)

corpus %>% texcur::tf_merge_tokens()
