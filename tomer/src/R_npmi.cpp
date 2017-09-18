#include <Rcpp.h>
#include <iostream>
#include <chrono>

#include "def.h"
#include "util.h"
#include "topic_evaluator.h"
#include "topic_evaluator_util.h"
#include "tokenizer.h"

using namespace tomer;


// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi3_cpp(const Rcpp::StringVector& topics,
                                       const Rcpp::StringVector& documents,
                                       size_t window_size) {
  StringVector tmp_tops = Rcpp::as<StringVector>(topics);
  StringVector tmp_docs = Rcpp::as<StringVector>(documents);

  Rcpp::Rcout << "Start transforming..." << std::endl;
  std::chrono::steady_clock::time_point tbegin = std::chrono::steady_clock::now();
  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> tops = tokenizer.transform(tmp_tops);
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(tmp_docs);
  std::chrono::steady_clock::time_point tend = std::chrono::steady_clock::now();
  Rcpp::Rcout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(tend - tbegin).count() <<std::endl;

  Rcpp::Rcout << "Start counting words..." << std::endl;
  std::chrono::steady_clock::time_point cbegin = std::chrono::steady_clock::now();
  WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
  calculate_word_index_counts_and_window_count(docs, window_size, &data);
  std::chrono::steady_clock::time_point cend = std::chrono::steady_clock::now();
  Rcpp::Rcout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(cend - cbegin).count() <<std::endl;

  Rcpp::Rcout << "Start evaluating topics..." << std::endl;
  std::chrono::steady_clock::time_point ebegin = std::chrono::steady_clock::now();
  CompressedNpmiEvaluator evaluator(std::move(data.word_index_counts), data.window_count);
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  std::chrono::steady_clock::time_point eend = std::chrono::steady_clock::now();
  Rcpp::Rcout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(eend - ebegin).count() <<std::endl;

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi2_cpp(const Rcpp::List& topics,
                                       const Rcpp::StringVector& documents,
                                       size_t window_size) {
  Corpus itopics = convert_from_R(topics);
  Vector<String> tmp_docs = Rcpp::as<Vector<String>>(documents);

  Rcpp::Rcout << "Start transforming..." << std::endl;
  std::chrono::steady_clock::time_point tbegin = std::chrono::steady_clock::now();
  WordTokenizer tokenizer;
  Corpus docs = tokenizer.transform(tmp_docs);
  std::chrono::steady_clock::time_point tend = std::chrono::steady_clock::now();
  Rcpp::Rcout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(tend - tbegin).count() <<std::endl;

  Rcpp::Rcout << "Start counting words..." << std::endl;
  std::chrono::steady_clock::time_point cbegin = std::chrono::steady_clock::now();
  WordTopicEvaluatorData data(std::move(create_word_counts(itopics)));
  calculate_word_counts_and_window_count(docs, window_size, &data);
  std::chrono::steady_clock::time_point cend = std::chrono::steady_clock::now();
  Rcpp::Rcout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(cend - cbegin).count() <<std::endl;

  Rcpp::Rcout << "Start evaluating topics..." << std::endl;
  std::chrono::steady_clock::time_point ebegin = std::chrono::steady_clock::now();
  NpmiEvaluator evaluator(std::move(data.word_counts), data.window_count);
  auto ntopics = itopics.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : itopics)
    vals.push_back(evaluator.evaluate(topic));

  std::chrono::steady_clock::time_point eend = std::chrono::steady_clock::now();
  Rcpp::Rcout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(eend - ebegin).count() <<std::endl;

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_cpp(const Rcpp::StringVector& topics,
                                      const Rcpp::StringVector& documents,
                                      size_t window_size) {
  StringVector tmp_tops = Rcpp::as<StringVector>(topics);
  StringVector tmp_docs = Rcpp::as<StringVector>(documents);

  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> tops = tokenizer.transform(tmp_tops);
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(tmp_docs);

  WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
  calculate_word_index_counts_and_window_count(docs, window_size, &data);

  CompressedNpmiEvaluator evaluator(std::move(data.word_index_counts), data.window_count);
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}
