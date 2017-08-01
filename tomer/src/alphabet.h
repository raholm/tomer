#ifndef ALPHABET_H
#define ALPHABET_H

#include <map>
#include <memory>

class Alphabet {
public:
  using Token = std::string;
  using Type = std::size_t;
  using SPtr = std::shared_ptr<Alphabet>;
  using size_type = std::map<Token, Type>::size_type;

  Alphabet();
  Alphabet(const std::map<Token, Type>& alphabet);
  Alphabet(const Alphabet& other) = default;;
  Alphabet(Alphabet&& other) = default;;

  ~Alphabet() = default;

  Alphabet& operator=(const Alphabet& rhs) = default;
  Alphabet& operator=(Alphabet&& rhs) = default;

  Type add(const Token& token);

  bool has(const Type& type) const;
  bool has(const Token& token) const;

  const Token& at(const Type& position);
  const Type& at(const Token& position);

  size_type size() const;

private:
  Type next_type_;
  std::map<Token, Type> alphabet_;
  std::map<Type, Token> inv_alphabet_;

};

#endif // ALPHABET_H
