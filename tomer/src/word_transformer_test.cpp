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
                       {0, 2, 3, WordToIndexTransformer::unobserved_word_index, 1, 0});
        expect_true(transformer.transform("hello") == 0);
        expect_true(transformer.transform("batman") == WordToIndexTransformer::unobserved_word_index);

        // Check reversion
        check_equality(transformer.revert({0, 2, 3, WordToIndexTransformer::unobserved_word_index, 1, 0}),
                       {"hello", "foo", "bar", WordToIndexTransformer::unobserved_word, "world", "hello"});
        expect_true(transformer.revert(0) == "hello");
        expect_true(transformer.revert(WordToIndexTransformer::unobserved_word_index) ==
                    WordToIndexTransformer::unobserved_word);
        expect_true(transformer.revert(4) == WordToIndexTransformer::unobserved_word);
      }
    }

  } // namespace test
} // namespace tomer
