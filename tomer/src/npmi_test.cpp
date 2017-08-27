#include <testthat.h>

#include "test_helper.h"
#include "npmi.h"

namespace tomer {
  namespace test {

    context("npmi") {
      test_that("word to index transformer works properly") {
        WordToIndexTransformer transformer;

        transformer.update("hello");
        transformer.update({"hello", "world", "foo", "bar"});

        // Check transformation
        check_equality(transformer.transform({"hello", "foo", "bar", "batman", "world", "hello"}),
                       {0, 2, 3, UNOBSERVED_WORDINDEX, 1, 0});
        expect_true(transformer.transform("hello") == 0);
        expect_true(transformer.transform("batman") == UNOBSERVED_WORDINDEX);
      }
    }

  } // namespace test
} // namespace tomer
