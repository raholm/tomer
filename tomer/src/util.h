#ifndef TOMER_UTIL_H_
#define TOMER_UTIL_H_

#include <Rcpp.h>
#include <iostream>

#include "def.h"

namespace tomer {

  template<typename T>
  inline void remove_duplicates(Vector<T>* const v) {
    Set<T> s;
    for (auto const& e : *v) s.insert(e);
    v->assign(s.begin(), s.end());
  }

  template<typename T>
  inline void remove_duplicates_inplace(Vector<T>* const v, size_t* const size) {
    Set<T> s;
    for (unsigned i = 0; i < *size; ++i) s.insert(v->at(i));
    size_t idx = 0;
    for (auto const& e : s) v->at(idx++) = e;
    *size = s.size();
  }

  template<typename T>
  inline void print(const std::vector<T>& v, const std::string& message) {
    std::cout << message << std::endl;
    for (auto const& e : v)
      std::cout << e << " ";
    std::cout << std::endl;
  }

  inline Corpus convert_from_R(const Rcpp::List& l) {
    return Rcpp::as<Corpus>(l);
  }

  inline DoubleVector convert_from_R(const Rcpp::NumericVector& v) {
    return Rcpp::as<DoubleVector>(v);
  }

  inline IntVector convert_from_R(const Rcpp::IntegerVector& v) {
    return Rcpp::as<IntVector>(v);
  }

  inline DoubleMatrix convert_from_R(const Rcpp::NumericMatrix& m) {
    unsigned nrow = m.nrow();
    DoubleMatrix mat(nrow);
    for (unsigned i = 0; i < nrow; ++i) {
      auto row = m(i, Rcpp::_);
      mat.at(i) = DoubleVector{row.begin(), row.end()};
    }
    return mat;
  }

  inline IntMatrix convert_from_R(const Rcpp::IntegerMatrix& m) {
    unsigned nrow = m.nrow();
    IntMatrix mat(nrow);
    for (unsigned i = 0; i < nrow; ++i) {
      auto row = m(i, Rcpp::_);
      mat.at(i) = IntVector{row.begin(), row.end()};
    }
    return mat;
  }

} // namespace tomer

#endif // TOMER_UTIL_H_
