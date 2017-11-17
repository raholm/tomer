#' @title
#' Creates an empty bayes factor mode
#'
#' @description
#' Creates an empty bayes factor mode.
#'
#' @importFrom methods new
#'
#' @export
get_bayes_factor_mode <- function() {
    module <- get_bayes_factor_module()
    class <- module$TypeTopicIndicatorMode
    new(class)
}

#' @title
#' Creates a bayes factor mode given data
#'
#' @description
#' Creates a bayes factor mode given a data frame of types and topic indicators.
#'
#' @param types A data frame of types and topic indicators.
#'
#' @export
get_bayes_factor_mode_from_data <- function(data) {
    checkmate::assert_subset(c("doc", "pos", "type", "topic"), names(data))
    checkmate::assert_integer(data$doc, lower=0)
    checkmate::assert_integer(data$pos, lower=0)
    checkmate::assert_integer(data$type, lower=0)
    checkmate::assert_integer(data$topic, lower=0)

    mode <- get_bayes_factor_mode()
    mode$update(data)
    mode
}

#' @title
#' Creates a bayes factor mode given a topic model
#'
#' @description
#' Creates a bayes factor mode given a topic model that generates \code{gibbs_iters} samples.
#'
#' @param model A topic model.
#' @param gibbs_iters An integer specifying how many sampling to do.
#' @param dst A filename to where the model will write its samples temporarily.
#'
#' @export
get_bayes_factor_mode_from_model <- function(model, gibbs_iters, dst) {
    checkmate::assert_numeric(gibbs_iters, len=1, lower=1)

    mode <- get_bayes_factor_mode()

    for (i in 1:gibbs_iters) {
        samples <- generate_samples_from(model, dst)
        mode$update(samples)
    }

    mode
}

#' @title
#' Writes a bayes factor mode to file
#'
#' @description
#' Writes a bayes factor mode to specified file location.
#'
#' @param mode A bayes factor mode.
#' @param dst A filename to where the mode data will be written.
#'
#' @importFrom utils write.table
#'
#' @export
write_bayes_factor_mode_to_file <- function(mode, dst) {
    checkmate::assert_path_for_output(dst)

    data <- mode$data()
    write.table(x=data, file=dst, row.names=FALSE)
}

#' @title
#' Reads a bayes factor mode from file
#'
#' @description
#' Reads a bayes factor mode from specified file location.
#'
#' @param src A filename to where the mode data will be read.
#'
#' @importFrom utils read.table
#'
#' @export
read_bayes_factor_mode_from_file <- function(src) {
    checkmate::assert_true(endsWith(src, ".mode"))

    data <- read.table(src, header=T)
    get_bayes_factor_mode_from_data(data)
}
