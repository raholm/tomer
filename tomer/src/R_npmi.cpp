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

    WordIndexTokenizerCache tokenizer{cache.transformer};
    tops = tokenizer.transform(tmp_tops);
    docs = tokenizer.transform(tmp_docs);
  } else {
    // Create cache
    WordIndexTokenizer tokenizer;
    tops = tokenizer.transform(tmp_tops);
    docs = tokenizer.transform(tmp_docs);

    WordToIndexTransformerCache  transformer_cache;
    WordToIndexTransformer transformer = tokenizer.get_transformer();
    Map<Word, WordIndex> index_map = transformer.get_index_map();

    for (auto const& word_wordindex : index_map)
      transformer_cache.update(word_wordindex.first, word_wordindex.second);

    WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
    calculate_word_index_counts_and_window_count(docs, window_size, &data);

    WordIndexCounterCache counter_cache;
    Map<WordIndex, Count> count_map = data.word_index_counts.get_count_map();

    for (auto const& wordindex_count : count_map)
      counter_cache.update(wordindex_count.first, wordindex_count.second);

    cache.transformer = std::move(transformer_cache);
    cache.word_index_counts = std::move(counter_cache);
    cache.window_count = data.window_count;

    bool status_ok = WordIndexTopicEvaluatorDataCacheWriter().write(cache, tmp_filename);
    if (!status_ok)
      throw std::runtime_error("Could not write out cache to disk.");
  }
  // ----

  CompressedAndCachedNpmiEvaluator evaluator(std::move(cache.word_index_counts), cache.window_count);
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}
