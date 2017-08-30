#ifndef TOMER_UTIL_H_
#define TOMER_UTIL_H_

#include <iostream>

namespace tomer {

  template<typename T>
    void remove_duplicates(Vector<T>& v) {
    Set<T> s;
    for (auto const& e : v) s.insert(e);
    v.assign(s.begin(), s.end());
  }

  template<typename T>
  void remove_duplicates(Vector<T>& v, size_t& size) {
    Set<T> s;
    for (unsigned i = 0; i < size; ++i) s.insert(v.at(i));
    size_t idx = -1;
    for (auto const& e : s) v.at(++idx) = e;
    size = idx;
  }

  template<typename T>
  inline void print(const std::vector<T>& v, const std::string& message) {
    std::cout << message << std::endl;
    for (auto const& e : v)
      std::cout << e << " ";
    std::cout << std::endl;
  }

} // namespace tomer

#endif // TOMER_UTIL_H_
