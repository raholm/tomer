#include <testthat.h>

#include "test_helper.h"
#include "counter.h"

namespace tomer {
  namespace test {

    context("word counter") {
      test_that("WordCounter works properly") {
        WordToIndexTransformer transformer;
        transformer.update({"foo", "bar", "batman", "superman"});

        TopicWordIndexRelationMap relations;
        relations.update(transformer.transform("foo"), transformer.transform("bar"));
        relations.update(transformer.transform("batman"), transformer.transform("superman"));
        relations.update(transformer.transform("superman"), transformer.transform(Vector<Word>{"foo", "bar"}));

        WordCounter word_counts{transformer, relations};

        expect_true(word_counts.get_count("foo") == 0);
        word_counts.update("foo");
        expect_true(word_counts.get_count("foo") == 1);

        word_counts.update("foo", "foo");
        expect_true(word_counts.get_count("foo", "foo") == 0);

        word_counts.update("superman", "batman");
        expect_true(word_counts.get_count("superman") == 0);
        expect_true(word_counts.get_count("superman", "batman") == 1);

        word_counts.update("batman", "superman");
        expect_true(word_counts.get_count("batman") == 0);
        expect_true(word_counts.get_count("batman", "superman") == 2);

        word_counts.update("foo");
        word_counts.update("foo");
        expect_true(word_counts.get_count("foo") == 3);

        word_counts.update("bar", "batman");
        expect_true(word_counts.get_count("bar", "batman") == 0);

        expect_true(word_counts.get_count("spiderman") == 0);
        word_counts.update("spiderman");
        expect_true(word_counts.get_count("spiderman") == 0);
      }
    }

  } // namespace test
} // namespace tomer
