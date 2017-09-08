#include <Rcpp.h>

#include "def.h"
#include "util.h"
#include "document_sampler.h"

using namespace tomer;

// [[Rcpp::export]]
Rcpp::DataFrame sample_document_cpp(size_t length,
                                    const Rcpp::NumericVector& topic_probs,
                                    const Rcpp::NumericMatrix& topic_type_probs) {

  SampledDocument document = sample_document(length,
                                             convert_from_R(topic_probs),
                                             convert_from_R(topic_type_probs));
  IntVector doc_ids(length, document.id);
  return Rcpp::DataFrame::create(Rcpp::Named("docid") = doc_ids,
                                 Rcpp::Named("type") = document.types,
                                 Rcpp::Named("topic") = document.topics);
}

// [[Rcpp::export]]
Rcpp::DataFrame sample_corpus_cpp(const Rcpp::IntegerVector length,
                                  const Rcpp::NumericMatrix& topic_probs,
                                  const Rcpp::NumericMatrix& topic_type_probs) {
  SampledCorpus corpus = sample_corpus(convert_from_R(length),
                                       convert_from_R(topic_probs),
                                       convert_from_R(topic_type_probs));
  return Rcpp::DataFrame::create(Rcpp::Named("docid") = corpus.doc_ids,
                                 Rcpp::Named("type") = corpus.types,
                                 Rcpp::Named("topic") = corpus.topics);
}
