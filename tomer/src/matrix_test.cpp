#include <testthat.h>

#include "test_helper.h"
#include "matrix.h"

namespace tomer {
  namespace test {

    context("lower triangular matrix") {
      test_that("LowerTriangularMatrix throws out_of_range errors correctly") {
        const LowerTriangularMatrix<int> m1(0);
        expect_error_as(m1.at(0, 0), std::out_of_range);

        const LowerTriangularMatrix<int> m2(2);
        expect_error_as(m2.at(2, 0), std::out_of_range);
        expect_error_as(m2.at(0, 2), std::out_of_range);
        expect_no_error(m2.at(0, 0));
        expect_no_error(m2.at(1, 0));
        expect_no_error(m2.at(0, 1));
        expect_no_error(m2.at(1, 1));
      }

      test_that("LowerTriangularMatrix setters/getters works correctly") {
        LowerTriangularMatrix<int> m2(2);

        m2.at(0, 0) = 1;
        expect_error_as(m2.at(0, 1) = 2, std::out_of_range);
        m2.at(1, 0) = 4;
        m2.at(1, 1) = 3;

        expect_true(m2.at(0, 0) == 1);
        expect_error_as(m2.at(0, 1), std::out_of_range);
        expect_true(m2.at(1, 0) == 4);
        expect_true(m2.at(1, 1) == 3);
      }

      test_that("get_size works properly") {
        LowerTriangularMatrix<int> m1(0);
        LowerTriangularMatrix<int> m2(100);

        expect_true(m1.size() == 0);
        expect_true(m2.size() == 100);
      }
    }

  } // namespace test
} // namespace tomer
