#ifndef TOMER_TOKENIZER_H_
#define TOMER_TOKENIZER_H_

#include <cstring>

#include "def.h"

namespace tomer {

  class Tokenizer {
  public:
    Tokenizer(const String& delimiter)
      : cache_size_{5000},
        delimiter_{delimiter} {}

    Corpus transform(const Vector<String>& texts) const {
      Corpus corpus;
      corpus.reserve(texts.size());
      for (auto const& text : texts)
        corpus.push_back(transform(text));
      return corpus;
    }

    Document transform(const String& text) const {
      Document document;
      document.reserve(cache_size_);

      char* t = const_cast<char*>(text.c_str());
      char* current_token = strtok(t, delimiter_.c_str());

      while (current_token != NULL) {
        if (*current_token != ' ')
          document.push_back(String(current_token));
        current_token = strtok(NULL, delimiter_.c_str());
      }

      document.shrink_to_fit();
      return document;
    }

  private:
    size_t cache_size_;
    String delimiter_;

  };

} // namespace tomer

#endif // TOMER_TOKENIZER_H_
