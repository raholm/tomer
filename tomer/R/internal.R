generate_samples_from <- function(model, filename) {
    if (file.exists(filename)) file.remove(filename)

    model$writeState(filename)
    tbl <- read.table(filename)
    names(tbl) <- c("doc", "source", "pos", "type", "token", "topic")

    if (file.exists(filename)) file.remove(filename)

    tbl %>%
        dplyr::select(doc, pos, type, topic)
}

get_bayes_factor_module <- function() {
    Rcpp::Module("mod_bayes_factor", "tomer")
}
