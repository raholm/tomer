#include <Rcpp.h>
#include <exception>

#include <iostream>
#include <chrono>

#include "def.h"
#include "util.h"
#include "topic_evaluator.h"
#include "topic_evaluator_util.h"
#include "topic_evaluator_cache.h"
#include "tokenizer.h"

using namespace tomer;

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

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_with_cache_cpp(const Rcpp::StringVector& topics,
                                                 const Rcpp::StringVector& documents,
                                                 size_t window_size,
                                                 const Rcpp::CharacterVector& filename) {
  StringVector tmp_tops = Rcpp::as<StringVector>(topics);
  StringVector tmp_docs = Rcpp::as<StringVector>(documents);
  String tmp_filename = Rcpp::as<String>(filename);

  WordIndexTopicEvaluatorDataCache cache;
  Matrix<WordIndexTokenizer::Token> tops;
  Matrix<WordIndexTokenizer::Token> docs;

  // Cache related stuff
  if (file_exists(tmp_filename)) {
    // Cache exists
    cache = WordIndexTopicEvaluatorDataCacheReader().read(tmp_filename);

    WordTokenizer word_tokenizer;
    Matrix<WordTokenizer::Token> word_tops = word_tokenizer.transform(tmp_tops);
    Matrix<WordTokenizer::Token> word_docs = word_tokenizer.transform(tmp_docs);

    tops = cache.transformer.transform(word_tops);
    docs = cache.transformer.transform(word_docs);
  } else {
    // Create cache
    WordIndexTokenizer tokenizer;
    tops = tokenizer.transform(tmp_tops);
    docs = tokenizer.transform(tmp_docs);

    WordTokenizer word_tokenizer;
    Matrix<WordTokenizer::Token> word_tops = word_tokenizer.transform(tmp_tops);
    Matrix<WordTokenizer::Token> word_docs = word_tokenizer.transform(tmp_docs);

    WordToIndexTransformerCache  transformer_cache;

    for (auto topic : word_tops)
      for (auto word : topic)
        transformer_cache.update(word, tokenizer.transform(word).at(0));

    for (auto doc : word_docs)
      for (auto word : doc)
        transformer_cache.update(word, tokenizer.transform(word).at(0));

    cache.transformer = std::move(transformer_cache);

    WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
    calculate_word_index_counts_and_window_count_with_cache(docs, window_size, &data, &cache);

    bool status_ok = WordIndexTopicEvaluatorDataCacheWriter().write(cache, tmp_filename);
    if (!status_ok)
      throw std::runtime_error("Could not write out cache to disk.");
  }

  CompressedAndCachedNpmiEvaluator evaluator(std::move(cache.word_index_counts), cache.window_count);
  // ----

  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}
