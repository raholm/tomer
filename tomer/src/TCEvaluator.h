#ifndef TC_EVALUATOR_H_
#define TC_EVALUATOR_H_

#include <fstream>

#include "def.h"
#include "util.h"
#include "tokenizer.h"
#include "word_transformer.h"

namespace tomer {

  template <typename T>
  class LowerTriangularMatrix {
  public:
    explicit LowerTriangularMatrix(size_t n, T val=0)
      : size_{n},
        values_(n + n * (n - 1) / 2, val) {}

    T at(size_t row, size_t col) const {
      check(row, col);
      return values_.at(get_index(row, col));
    }

    T& at(size_t row, size_t col) {
      check(row, col);
      return values_.at(get_index(row, col));
    }

  private:
    size_t size_;
    Vector<T> values_;

    void check(size_t row, size_t col) {
      if (row >= size_ || col >= size_)
        throw new std::out_of_range("Out of range.");
    }

    size_t get_index(size_t row, size_t col) {
      return row > col ? calculate_index(row, col) : calculate_index(col, row);
    }

    size_t calculate_index(size_t row, size_t col) {
      return ((row + 1) * row / 2 + col);
    }

  };

  class TCEvaluator {
  public:
    explicit TCEvaluator(const String& reference_file)
      : n_tokens_{0},
        reference_file_{reference_file},
        tokenizer_{},
        word_counts_{0}
    {
      if (!file_exists(reference_file))
        throw new std::invalid_argument("Reference file does not exist.");
    }

    double evaluate(const StringVector& topic_words) {
      tokenizer_ = WordIndexTokenizer();

      Matrix<WordIndex> topic_word_indexes = tokenizer_.transform(topic_words);
      n_tokens_ = tokenizer_.get_transformer().get_indexes().size();

      word_counts_ = LowerTriangularMatrix<Count>(n_tokens_);
      fill_word_counts();

      return 0;
    }

  private:
    int n_tokens_;
    String reference_file_;
    WordIndexTokenizer tokenizer_;
    LowerTriangularMatrix<Count> word_counts_;

    void fill_word_counts() {
      std::ifstream file{reference_file_};
      String document;
      size_t document_size;
      WordIndex current_word_index, other_word_index;
      Vector<WordIndex> current_word_indexes;

      if (file.fail()) {
        throw new std::runtime_error("Reference file could not be opened.");
      }

      while (std::getline(file, document)) {
        current_word_indexes = tokenizer_.transform(document);
        document_size = current_word_indexes.size();
        remove_duplicates_inplace(&current_word_indexes, &document_size);

        for (unsigned current_idx = 0; current_idx < document_size; ++current_idx) {
          current_word_index = current_word_indexes.at(current_idx);

          if (is_interesting(current_word_index)) {
            for (unsigned other_idx = 0; other_idx < document_size; ++other_idx) {
              other_word_index = current_word_indexes.at(other_idx);

              if (is_interesting(other_word_index))
                ++word_counts_.at(current_word_index, other_word_index);
            }
          }
        }
      }

      file.close();
    }

    bool is_interesting(const WordIndex& word_index) {
      return word_index != WordToIndexTransformer::unobserved_word_index && word_index < n_tokens_;
    }

  };
} // namespace tomer

#endif // TC_EVALUATOR_H_
