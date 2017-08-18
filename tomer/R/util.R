tokens_to_topic_indicators <- function(tokens, mode) {
    mode$tokens_to_topic_indicators(tokens)
}

get_mode_from_data <- function(tokens, topic_indicators) {
    mode <- get_object_of("TypeTopicIndicatorMode", "mod_bayes_factor")
    mode$update(tokens, topic_indicators)
    mode
}

get_mode_from_model <- function(model, gibbs_iters) {
    mode <- get_object_of("TypeTopicIndicatorMode", "mod_bayes_factor")

    for (i in 1:gibbs_iters) {
        samples <- generate_samples_from(model)
        mode$update(samples$token, samples$topic)
    }

    mode
}

generate_samples_from <- function(model, filename) {
    if (file.exists(filename)) file.remove(filename)

    model$writeState(filename)
    tbl <- read.table(filename)
    names(tbl) <- c("doc", "source", "pos", "type", "token", "topic")

    if (file.exists(filename)) file.remove(filename)

    list(token=as.character(tbl$token), topic=as.numeric(tbl$topic))
}

get_module <- function(module) {
    Module(module, "tomer")
}

get_object_of <- function(class, module) {
    module <- get_module(module)
    class <- module[[class]]
    new(class)
}
