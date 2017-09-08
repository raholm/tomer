#include <Rcpp.h>

#include "def.h"
#include "document_sampler.h"

Rcpp::DataFrame sample_document_cpp(size_t length,
                                    const Rcpp::NumericVector& topic_probs,
                                    const Rcpp::NumericMatrix& topic_type_probs) {

}

Rcpp::DataFrame sample_corpus_cpp(const Rcpp::IntegerVector length,
                                  const Rcpp::NumericVector& topic_probs,
                                  const Rcpp::NumericMatrix& topic_type_probs) {

}

Rcpp::DataFrame sample_corpus_cpp(const Rcpp::IntegerVector length,
                                  const Rcpp::NumericMatrix& topic_probs,
                                  const Rcpp::NumericMatrix& topic_type_probs) {

}
