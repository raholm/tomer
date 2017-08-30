#include <testthat.h>
#include <algorithm>

#include "test_helper.h"
#include "util.h"

namespace tomer {
  namespace test {

    context("utility functions") {
      test_that("remove duplicates works properly") {
        Vector<int> v{1, 2, 1, 2, 3, 3, 1, 2, 1};
        remove_duplicates(&v);
        std::sort(v.begin(), v.end());
        check_equality(v, {1, 2, 3});
      }

      test_that("remove duplicates inplace works properly") {
        Vector<int> v{1, 2, 1, 2, 3, 3, 1, 2, 1};
        size_t size = 5;
        remove_duplicates_inplace(&v, &size);
        expect_true(size == 3);
        expect_true(v.at(0) == 1);
        expect_true(v.at(1) == 2);
        expect_true(v.at(2) == 3);
      }
    }

  } // namespace test
} // namespace tomer
