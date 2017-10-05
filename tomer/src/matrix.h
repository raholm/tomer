#ifndef TOMER_MATRIX_H_
#define TOMER_MATRIX_H_

#include <exception>

namespace tomer {

  template <typename T>
  class LowerTriangularMatrix {
  public:
    explicit LowerTriangularMatrix(size_t n, T val=0)
      : size_{n},
        default_val_{val},
        values_(n + n * (n - 1) / 2, val) {}

    const T& at(size_t row, size_t col) const {
      if (is_out_of_range(row, col))
        throw std::out_of_range("Out of range.");
      auto index = get_index(row, col);
      if (index == -1) return default_val_;
      return values_.at(index);
    }

    T& at(size_t row, size_t col) {
      if (is_out_of_range(row, col))
        throw std::out_of_range("Out of range.");
      auto index = get_index(row, col);
      if (index == -1)
        throw std::out_of_range("Cannot access upper triangular part.");
      return values_.at(index);
    }

    size_t size() const {
      return size_;
    }

  private:
    size_t size_;
    T default_val_;
    Vector<T> values_;

    bool is_out_of_range(size_t row, size_t col) const {
      return row >= size_ || col >= size_;
    }

    int get_index(size_t row, size_t col) const {
      return row >= col ? calculate_index(row, col) : -1;
    }

    size_t calculate_index(size_t row, size_t col) const {
      return ((row + 1) * row / 2 + col);
    }

  };

} // namespace tomer

#endif // TOMER_MATRIX_H_
