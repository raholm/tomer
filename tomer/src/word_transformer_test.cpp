#include <testthat.h>

#include "test_helper.h"
#include "word_transformer.h"

namespace tomer {
  namespace test {

    context("word transformer") {
      test_that("WordToIndexTransformer works properly") {
        WordToIndexTransformer transformer;

        transformer.update("hello");
        transformer.update({"hello", "world", "foo", "bar"});

        // Check transformation
        check_equality(transformer.transform({"hello", "foo", "bar", "batman", "world", "hello"}),
                       {0, 2, 3, UNOBSERVED_WORDINDEX, 1, 0});
        expect_true(transformer.transform("hello") == 0);
        expect_true(transformer.transform("batman") == UNOBSERVED_WORDINDEX);

        // Check reversion
        check_equality(transformer.revert({0, 2, 3, UNOBSERVED_WORDINDEX, 1, 0}),
                       {"hello", "foo", "bar", UNKNOWN_WORD, "world", "hello"});
        expect_true(transformer.revert(0) == "hello");
        expect_true(transformer.revert(UNOBSERVED_WORDINDEX) == UNKNOWN_WORD);
        expect_true(transformer.revert(4) == UNKNOWN_WORD);
      }
    }

  } // namespace test
} // namespace tomer
