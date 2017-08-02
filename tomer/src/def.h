#ifndef DEF_H
#define DEF_H

#include <string>
#include <vector>
#include <map>
#include <memory>

using Token = std::string;
using Type = std::size_t;
using Topic = std::size_t;

using Document = std::vector<Token>;
using Corpus = std::vector<Document>;

using IntVector = std::vector<uint>;
using DoubleVector = std::vector<double>;
using StringVector = Document;
using TypeVector = std::vector<Type>;

using IntMatrix = std::vector<IntVector>;
using DoubleMatrix = std::vector<DoubleVector>;
using TypeMatrix = std::vector<TypeVector>;

#endif // DEF_H
