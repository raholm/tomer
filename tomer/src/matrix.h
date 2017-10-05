#ifndef TOMER_MATRIX_H_
#define TOMER_MATRIX_H_

namespace tomer {

  template <typename T>
  class LowerTriangularMatrix {
  public:
    explicit LowerTriangularMatrix(size_t n, T val=0)
      : size_{n},
        values_(n + n * (n - 1) / 2, val) {}

    T at(size_t row, size_t col) const {
      check(row, col);
      return values_.at(get_index(row, col));
    }

    T& at(size_t row, size_t col) {
      check(row, col);
      return values_.at(get_index(row, col));
    }

    size_t get_size() const {
      return size_;
    }

    private:
    size_t size_;
    Vector<T> values_;

    void check(size_t row, size_t col) const {
      if (row >= size_ || col >= size_)
        throw new std::out_of_range("Out of range.");
    }

    size_t get_index(size_t row, size_t col) const {
      return row > col ? calculate_index(row, col) : calculate_index(col, row);
    }

    size_t calculate_index(size_t row, size_t col) const {
      return ((row + 1) * row / 2 + col);
    }

  };

} // namespace tomer

#endif // TOMER_MATRIX_H_
