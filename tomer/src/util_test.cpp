#include <testthat.h>
#include <algorithm>

#include "test_helper.h"
#include "util.h"

namespace tomer {
  namespace test {

    context("utility functions") {
      test_that("remove duplicates works properly") {
        Vector<int> v{1, 2, 1, 2, 3, 3, 1, 2, 1};
        remove_duplicates(v);
        std::sort(v.begin(), v.end());
        check_equality(v, {1, 2, 3});
      }
    }

  } // namespace test
} // namespace tomer
