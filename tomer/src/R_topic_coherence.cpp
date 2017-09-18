#include <Rcpp.h>

#include "def.h"
#include "util.h"
#include "tokenizer.h"
#include "topic_evaluator.h"
#include "topic_evaluator_util.h"

using namespace tomer;

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_topic_coherence2_cpp(const Rcpp::List& topics,
                                                 const Rcpp::List& documents) {
  Corpus tops = convert_from_R(topics);
  Corpus docs = convert_from_R(documents);

  WordTopicEvaluatorData data(std::move(create_word_counts(tops)));
  calculate_word_counts_and_window_count(docs, INF_WORD_WINDOW, &data);

  TopicCoherenceEvaluator evaluator(data.word_counts);
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_topic_coherence_cpp(const Rcpp::StringVector& topics,
                                      const Rcpp::StringVector& documents) {
  StringVector tmp_tops = Rcpp::as<StringVector>(topics);
  StringVector tmp_docs = Rcpp::as<StringVector>(documents);

  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> tops = tokenizer.transform(tmp_tops);
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(tmp_docs);

  WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
  calculate_word_index_counts_and_window_count(docs, INF_WORD_WINDOW, &data);

  CompressedTopicCoherenceEvaluator evaluator(std::move(data.word_index_counts));
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}
