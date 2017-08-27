#ifndef TOMER_DEF_H_
#define TOMER_DEF_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace tomer {

  using String = std::string;
  using size_t = std::size_t;

  template<typename T>
  using Vector = std::vector<T>;

  template<typename Key, typename Value>
  using Map = std::map<Key, Value>;

  template<typename T>
  using Set = std::set<T>;

  template<typename First, typename Second>
  using Pair = std::pair<First, Second>;

  using Token = String;
  using Type = size_t;
  using Topic = size_t;

  using Document = Vector<Token>;
  using Corpus = Vector<Document>;

  using IntVector = Vector<size_t>;
  using DoubleVector = Vector<double>;
  using StringVector = Document;
  using TypeVector = Vector<Type>;

  using IntMatrix = Vector<IntVector>;
  using DoubleMatrix = Vector<DoubleVector>;
  using TypeMatrix = Vector<TypeVector>;

  // NPMI
  using WordIndex = int;
  using Word = String;
  using Count = size_t;


} // namespace tomer


#endif // DEF_H
