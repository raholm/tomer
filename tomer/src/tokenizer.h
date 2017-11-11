#ifndef TOMER_TOKENIZER_H_
#define TOMER_TOKENIZER_H_

#include <cstring>
#include <Rcpp.h>

#include "def.h"
#include "word_transformer.h"

namespace tomer {

  template<typename T>
  class Tokenizer {
  public:
    using Token = T;

    explicit Tokenizer(const String& delimiter, size_t cache_size)
      : delimiter_{delimiter},
        cache_size_{cache_size} {}

    virtual ~Tokenizer() = default;

    Matrix<Token> transform(const StringVector& texts) const {
      Matrix<Token> tokens;
      tokens.reserve(texts.size());

      for (auto const& text : texts)
        tokens.push_back(transform(text));

      return tokens;
    }

    Vector<Token> transform(const String& text) const {
      Vector<Token> tokens;
      tokens.reserve(cache_size_);

      char* t = new char[text.length() + 1];
      std::strcpy(t, text.c_str());

      char* current_token = strtok(t, delimiter_.c_str());

      while (current_token != NULL) {
        tokens.push_back(transform_token(current_token));
        current_token = strtok(NULL, delimiter_.c_str());
      }

      delete [] t;

      tokens.shrink_to_fit();
      return tokens;
    }

    Matrix<Token> transform(const Rcpp::StringVector& texts) const {
      Matrix<Token> tokens;
      tokens.reserve(texts.size());

      for (auto const& text : texts)
        tokens.push_back(transform(Rcpp::as<String>(text)));

      return tokens;
    }

  protected:
    virtual Token transform_token(char* token) const = 0;

  private:
    String delimiter_;
    size_t cache_size_;

  };

  class WordTokenizer : public Tokenizer<Word> {
  public:
    using BaseClass = Tokenizer<Word>;
    using Token = BaseClass::Token;

    WordTokenizer(const String& delimiter=" ", size_t cache_size=4096)
      : BaseClass{delimiter, cache_size} {}

  protected:
    Token transform_token(char* token) const override {
      return String{token};
    }

  };

  class WordIndexTokenizer : public Tokenizer<WordIndex> {
  public:
    using BaseClass = Tokenizer<WordIndex>;
    using Token = BaseClass::Token;

    WordIndexTokenizer(const String& delimiter=" ", size_t cache_size=4096)
      : BaseClass{delimiter, cache_size},
        transformer_{} {}

    WordIndexTokenizer& operator=(const WordIndexTokenizer& other) = default;
    WordIndexTokenizer& operator=(WordIndexTokenizer&& other) = default;

    const WordToIndexTransformer& get_transformer() const {
      return transformer_;
    }

  protected:
    Token transform_token(char* token) const override {
      return transformer_.update_and_transform(Word{token});
    }

  private:
    mutable WordToIndexTransformer transformer_;

  };

  class FixedWordIndexTokenizer : public Tokenizer<WordIndex> {
  public:
    using BaseClass = Tokenizer<WordIndex>;
    using Token = BaseClass::Token;

    FixedWordIndexTokenizer(const String& delimiter=" ",
                            size_t cache_size=4096)
      : BaseClass{delimiter, cache_size},
        transformer_{} {}

    FixedWordIndexTokenizer& operator=(const FixedWordIndexTokenizer& other) = default;
    FixedWordIndexTokenizer& operator=(FixedWordIndexTokenizer&& other) = default;

    const FixedWordToIndexTransformer& get_transformer() const {
      return transformer_;
    }

    void set_full() {
      transformer_.set_full();
    }

  protected:
    Token transform_token(char* token) const override {
      return transformer_.update_and_transform(Word{token});
    }

  private:
    mutable FixedWordToIndexTransformer transformer_;

  };

  class WordIndexTokenizerCache : public Tokenizer<WordIndex> {
  public:
    using BaseClass = Tokenizer<WordIndex>;
    using Token = BaseClass::Token;

    WordIndexTokenizerCache(const WordToIndexTransformerCache& transformer,
                            const String& delimiter=" ", size_t cache_size=4096)
      : BaseClass{delimiter, cache_size},
        transformer_{transformer} {}
    WordIndexTokenizerCache(WordToIndexTransformerCache&& transformer,
                            const String& delimiter=" ", size_t cache_size=4096)
      : BaseClass{delimiter, cache_size},
        transformer_{std::move(transformer)} {}

    const WordToIndexTransformerCache& get_transformer() const {
      return transformer_;
    }

  protected:
    Token transform_token(char* token) const override {
      return transformer_.transform(Word{token});
    }

  private:
    WordToIndexTransformerCache transformer_;

  };

} // namespace tomer

#endif // TOMER_TOKENIZER_H_
