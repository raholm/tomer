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
  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> tops = tokenizer.transform(topics);
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(documents);

  WordIndexTopicEvaluatorData data(create_word_index_counts(tops));
  calculate_word_counts_and_window_count(docs, window_size, &data);

  CompressedNpmiEvaluator evaluator(std::move(data.word_counts), data.window_count);
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
  String tmp_filename = Rcpp::as<String>(filename);

  WordIndexTopicEvaluatorDataCache cache;
  Matrix<WordIndexTokenizer::Token> tops;
  Matrix<WordIndexTokenizer::Token> docs;

  // Cache related stuff
  if (file_exists(tmp_filename)) {
    // Cache exists
    cache = WordIndexTopicEvaluatorDataCacheReader().read(tmp_filename);

    WordIndexTokenizerCache tokenizer{std::move(cache.transformer)};
    tops = tokenizer.transform(topics);
    docs = tokenizer.transform(documents);
  } else {
    // Create cache
    WordIndexTokenizer tokenizer;
    tops = tokenizer.transform(topics);
    docs = tokenizer.transform(documents);

    WordToIndexTransformerCache  transformer_cache{std::move(tokenizer.get_transformer())};

    WordIndexTopicEvaluatorData data(create_word_index_counts(tops));
    calculate_word_counts_and_window_count(docs, window_size, &data);

    WordIndexCounterCache counter_cache{data.word_counts};

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

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_from_file_cpp(const Rcpp::StringVector& topics,
                                                size_t window_size,
                                                const Rcpp::CharacterVector& filename) {

  FixedWordIndexTokenizer tokenizer;
  Matrix<WordIndex> topic_word_indexes = tokenizer.transform(Rcpp::as<StringVector>(topics));
  tokenizer.set_full();
  size_t n_tokens = tokenizer.get_transformer().get_indexes().size();

  SparseWordIndexTopicEvaluatorData data{SparseWordIndexCounter(n_tokens)};
  calculate_word_counts_and_window_count(Rcpp::as<String>(filename),
                                         window_size,
                                         tokenizer,
                                         &data);

  SparseCompressedNpmiEvaluator evaluator(std::move(data.word_index_counts), data.window_count);

  auto ntopics = topic_word_indexes.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : topic_word_indexes)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}


// [[Rcpp::export]]
void create_word_count_cache_cpp(const Rcpp::StringVector& documents,
                                 size_t window_size,
                                 const Rcpp::CharacterVector& filename) {
  if (file_exists(Rcpp::as<String>(filename)))
    throw std::runtime_error("File already exists.");

  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(documents);
  WordToIndexTransformerCache  transformer_cache{std::move(tokenizer.get_transformer())};

  if (window_size == INF_WORD_WINDOW) {
    for (auto const doc : docs) {
      window_size = std::max(window_size, doc.size());
    }
  }

  auto ndocs = documents.size();
  size_t nwindows, window_count = 0;
  WordIndexWindow words_in_window(window_size);
  WordIndexCounterCache word_index_counts;

  size_t head_id, tail_id, nwords;

  for (unsigned i = 0; i < ndocs; ++i) {
    auto doc_words = docs.at(i);
    auto doc_length = doc_words.size();

    if (window_size == INF_WORD_WINDOW)
      nwindows = 1;
    else
      nwindows = doc_length + window_size - 1;

    window_count += nwindows;

    for (unsigned j = 1; j < (nwindows + 1); ++j) {
      if (window_size == INF_WORD_WINDOW) {
        nwords = doc_words.size();

        for (unsigned k = 0; k < nwords; ++k) {
          words_in_window.at(k) = doc_words.at(k);
        }

        remove_duplicates_inplace(&words_in_window, &nwords);
      } else {
        head_id = (j > window_size) ? j - window_size : 0;
        tail_id = std::min((size_t) j, (size_t) doc_words.size());
        nwords = tail_id - head_id;

        for (unsigned k = 0; k < nwords; ++k) {
          words_in_window.at(k) = doc_words.at(head_id + k);
        }

        remove_duplicates_inplace(&words_in_window, &nwords);
      }

      for (unsigned left_idx = 0; left_idx < nwords; ++left_idx) {
        auto left_word = words_in_window.at(left_idx);
        word_index_counts.update(left_word);

        for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
          auto right_word = words_in_window.at(right_idx);
          word_index_counts.update(left_word, right_word);
        }
      }
    }
  }

  WordIndexTopicEvaluatorDataCache cache{std::move(transformer_cache), std::move(word_index_counts), window_size};
  bool status_ok = WordIndexTopicEvaluatorDataCacheWriter().write(cache, Rcpp::as<String>(filename));
  if (!status_ok)
    throw std::runtime_error("Could not write out cache to disk.");
}
