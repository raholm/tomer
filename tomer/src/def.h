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

  template<typename T>
  using Matrix = std::vector<std::vector<T>>;

  using Token = String;
  using Type = size_t;
  using Topic = size_t;

  using Document = Vector<Token>;
  using Corpus = Vector<Document>;

  using IntVector = Vector<size_t>;
  using DoubleVector = Vector<double>;
  using StringVector = Vector<String>;
  using TypeVector = Vector<Type>;
  using TopicVector = Vector<Topic>;

  using IntMatrix = Matrix<size_t>;
  using DoubleMatrix = Matrix<double>;
  using TypeMatrix = Matrix<Type>;
  using TopicMatrix = Matrix<Topic>;

  // NPMI
  using WordIndex = int;
  using Word = Token;
  using Count = size_t;
  using WordWindow = Vector<Word>;
  using WordIndexWindow = Vector<WordIndex>;

} // namespace tomer

#endif // TOMER_DEF_H_
