#ifndef TOMER_TOKENIZER_H_
#define TOMER_TOKENIZER_H_

#include <cstring>

#include "def.h"
#include "word_transformer.h"

namespace tomer {

  template<typename T>
  class Tokenizer {
  public:
    using Token = T;

    virtual ~Tokenizer() = default;

    virtual Matrix<Token> transform(const StringVector& texts) const {
      Matrix<Token> tokens;
      tokens.reserve(texts.size());

      for (auto const& text : texts)
        tokens.push_back(transform(text));

      return tokens;
    }

    virtual Vector<Token> transform(const String& text) const = 0;

  };

  class WordTokenizer : public Tokenizer<Word> {
  public:
    using BaseClass = Tokenizer<Word>;
    using Token = BaseClass::Token;

    WordTokenizer(const String& delimiter=" ", size_t cache_size=4096)
      : cache_size_{cache_size},
        delimiter_{delimiter} {}

    // TODO: Wtf is going on here? Why is this required?
    Matrix<Token> transform(const StringVector& texts) const override {
      return BaseClass::transform(texts);
    }

    Vector<Token> transform(const String& text) const override {
      Vector<Token> words;
      words.reserve(cache_size_);

      char* t = const_cast<char*>(text.c_str());
      char* current_token = strtok(t, delimiter_.c_str());

      while (current_token != NULL) {
        if (*current_token != ' ')
          words.push_back(String(current_token));
        current_token = strtok(NULL, delimiter_.c_str());
      }

      words.shrink_to_fit();
      return words;
    }

  private:
    size_t cache_size_;
    String delimiter_;

  };

  class WordIndexTokenizer : public Tokenizer<WordIndex> {
  public:
    using BaseClass = Tokenizer<WordIndex>;
    using Token = BaseClass::Token;

    WordIndexTokenizer(const String& delimiter=" ", size_t cache_size=4096)
      : cache_size_{cache_size},
        delimiter_{delimiter},
        transformer_{} {}

    // TODO: Wtf is going on here? Why is this required?
    Matrix<Token> transform(const StringVector& texts) const override {
      return BaseClass::transform(texts);
    }

    Vector<Token> transform(const String& text) const override {
      Vector<Token> indexes;
      indexes.reserve(cache_size_);

      char* t = const_cast<char*>(text.c_str());
      char* current_token = strtok(t, delimiter_.c_str());

      while (current_token != NULL) {
        if (*current_token != ' ') {
          Word ct{current_token};
          Token index = transformer_.update_and_transform(ct);
          indexes.push_back(index);
        }

        current_token = strtok(NULL, delimiter_.c_str());
      }

      indexes.shrink_to_fit();
      return indexes;
    }

  private:
    size_t cache_size_;
    String delimiter_;
    mutable WordToIndexTransformer transformer_;

  };

} // namespace tomer

#endif // TOMER_TOKENIZER_H_
