#include <Rcpp.h>
#include <iostream>

#include "def.h"
#include "util.h"
#include "topic_evaluator_util.h"
#include "topic_evaluator.h"
#include "tokenizer.h"

using namespace tomer;

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi2_cpp(const Rcpp::List& topics,
                                      const Rcpp::StringVector& documents,
                                      size_t window_size) {
  Corpus itopics = convert_from_R(topics);
  Vector<String> tmp_docs = Rcpp::as<Vector<String>>(documents);

  Tokenizer tokenizer(" ");
  Corpus docs = tokenizer.transform(tmp_docs);

  TopicEvaluatorData data(std::move(create_word_counts(itopics)));
  calculate_word_counts_and_window_count(docs, window_size, &data);

  NpmiEvaluator evaluator(data.word_counts, data.window_count);
  auto ntopics = itopics.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : itopics)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}


// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_cpp(const Rcpp::List& topics,
                                      const Rcpp::List& documents,
                                      size_t window_size) {
  Corpus itopics = convert_from_R(topics);
  Corpus idocuments = convert_from_R(documents);

  TopicEvaluatorData data(std::move(create_word_counts(itopics)));
  calculate_word_counts_and_window_count(idocuments, window_size, &data);

  NpmiEvaluator evaluator(data.word_counts, data.window_count);
  auto ntopics = itopics.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : itopics)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}
