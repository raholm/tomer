#ifndef DEF_H
#define DEF_H

#include <string>
#include <vector>
#include <map>
#include <memory>

using Token = std::string;
using Type = std::size_t;

using Document = std::vector<Token>;
using Corpus = std::vector<Document>;

using TypeVector = std::vector<Type>;
using TypeMatrix = std::vector<TypeVector>;

using Alphabet = std::map<Token, Type, std::less<Token>>;
using AlphabetPtr = std::shared_ptr<Alphabet>;

using uint = std::size_t;

using UIntVector = std::vector<uint>;
using DoubleVector = std::vector<double>;

using UIntMatrix = std::vector<UIntVector>;
using DoubleMatrix = std::vector<DoubleVector>;

#endif // DEF_H
