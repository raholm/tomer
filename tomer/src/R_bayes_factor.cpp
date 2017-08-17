#include <Rcpp.h>

#include "def.h"
#include "bayes_factor.h"

using namespace tomer;

// [[Rcpp::export]]
double compute_log_bayes_factor_cpp(const Rcpp::IntegerVector& topic_indicators,
                                    size_t n_topics) {
  return compute_log_bayes_factor(Rcpp::as<IntVector>(topic_indicators),
                                  n_topics);
}
