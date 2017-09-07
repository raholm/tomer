#include <testthat.h>

#include "test_helper.h"
#include "topic_evaluator.h"

namespace tomer {
  namespace test {

    context("topic evaluator") {
      test_that("NormalisedPointwiseMutualInformationEvaluator works properly") {
        WordToIndexTransformer transformer;
        transformer.update({"foo", "bar", "batman", "superman"});

        TopicWordIndexRelationMap relations;
        relations.update(transformer.transform("foo"), transformer.transform("bar"));
        relations.update(transformer.transform("batman"), transformer.transform("superman"));
        relations.update(transformer.transform("superman"),
                         transformer.transform(Vector<Word>{"foo", "bar"}));

        WordCount word_counts{transformer, relations};
        word_counts.update(Vector<Word>{"foo", "foo", "bar", "bar", "batman",
              "batman", "batman", "superman"});

        expect_true(word_counts.get_count("foo") == 2);
        expect_true(word_counts.get_count("bar") == 2);
        expect_true(word_counts.get_count("batman") == 3);
        expect_true(word_counts.get_count("superman") == 1);

        word_counts.update(Vector<Word>{"foo", "bar", "bar", "batman", "batman", "superman"},
                           Vector<Word>{"bar", "foo", "bar", "superman", "superman", "foo"});

        expect_true(word_counts.get_count("foo", "bar") == 2);
        expect_true(word_counts.get_count("bar", "foo") == 2);
        expect_true(word_counts.get_count("batman", "superman") == 2);
        expect_true(word_counts.get_count("superman", "batman") == 2);
        expect_true(word_counts.get_count("superman", "foo") == 1);
        expect_true(word_counts.get_count("foo", "superman") == 1);
        expect_true(word_counts.get_count("superman", "bar") == 0);
        expect_true(word_counts.get_count("bar", "superman") == 0);
        expect_true(word_counts.get_count("foo", "batman") == 0);

        /*
          (foo, bar)
          count(foo) = 2
          count(bar) = 2
          count(foo, bar) = 2
          numerator = log((2 * 5) / (2 * 2)) = log(10 / 4) = log(5 / 2)
          denominator = -log(2 / 5)
          val = log(5 / 2) / -log(2 / 5)

          (foo, batman)
          count(foo) = 2
          count(batman) = 3
          count(foo, batman) = 0
          val = 0

          (foo, superman)
          count(foo) = 2
          count(superman) = 1
          count(foo, superman) = 1
          numerator = log((1 * 5) / (2 * 1)) = log(5 / 2)
          denominator = -log(1 / 5)
          val = log(5 / 2) / -log(1 / 5)

          (foo, spiderman)
          count(foo) = 2
          count(spiderman) = 0
          count(foo, spiderman) = 0
          val = 0

          (bar, batman)
          count(foo) = 2
          count(spiderman) = 0
          count(foo, spiderman) = 0
          val = 0

          (bar, superman)
          count(bar) = 2
          count(superman) = 1
          count(bar, superman) = 0
          val = 0

          (bar, spiderman)
          count(bar) = 2
          count(spiderman) = 0
          count(bar, spiderman) = 0
          val = 0

          (batman, superman)
          count(batman) = 3
          count(superman) = 1
          count(batman, superman) = 2
          numerator = log((2 * 5) / (3 * 1))
          denominator = -log(2 / 5)
          val = log(10 / 3) / -log(2 / 5)

          (batman, spiderman)
          count(batman) = 3
          count(spiderman) = 0
          count(batman, spiderman) = 0
          val = 0

          (superman, spiderman)
          count(superman) = 1
          count(spiderman) = 0
          count(superman, spiderman) = 0
          val = 0
        */

        size_t window_count = 5;
        NormalisedPointwiseMutualInformationEvaluator evaluator{word_counts, window_count};

        double actual = evaluator.evaluate({"foo", "bar", "batman", "superman", "spiderman"});
        double expected = log((double) 5 / 2) / -log((double) 2 / 5) +
          log((double) 5 / 2) / -log((double) 1 / 5) +
          log((double) 10 / 3) / -log((double) 2 / 5);
        expect_true(is_equal(actual, expected));
      }

      test_that("TopicCoherenceEvaluator works properly") {
        WordToIndexTransformer transformer;
        transformer.update({"foo", "bar", "batman", "superman"});

        TopicWordIndexRelationMap relations;
        relations.update(transformer.transform("foo"), transformer.transform("bar"));
        relations.update(transformer.transform("batman"), transformer.transform("superman"));
        relations.update(transformer.transform("superman"),
                         transformer.transform(Vector<Word>{"foo", "bar"}));

        WordCount word_counts{transformer, relations};
        word_counts.update(Vector<Word>{"foo", "foo", "bar", "bar", "batman",
              "batman", "batman", "superman"});

        expect_true(word_counts.get_count("foo") == 2);
        expect_true(word_counts.get_count("bar") == 2);
        expect_true(word_counts.get_count("batman") == 3);
        expect_true(word_counts.get_count("superman") == 1);

        word_counts.update(Vector<Word>{"foo", "bar", "bar", "batman", "batman", "superman"},
                           Vector<Word>{"bar", "foo", "bar", "superman", "superman", "foo"});

        expect_true(word_counts.get_count("foo", "bar") == 2);
        expect_true(word_counts.get_count("bar", "foo") == 2);
        expect_true(word_counts.get_count("batman", "superman") == 2);
        expect_true(word_counts.get_count("superman", "batman") == 2);
        expect_true(word_counts.get_count("superman", "foo") == 1);
        expect_true(word_counts.get_count("foo", "superman") == 1);
        expect_true(word_counts.get_count("superman", "bar") == 0);
        expect_true(word_counts.get_count("bar", "superman") == 0);
        expect_true(word_counts.get_count("foo", "batman") == 0);

        /*
          (foo, bar)
          count(foo) = 2
          count(bar) = 2
          count(foo, bar) = 2
          val = log((2 + 1) / 2) = log(3 / 2)

          (foo, batman)
          count(foo) = 2
          count(batman) = 3
          count(foo, batman) = 0
          val = log((0 + 1) / 2) = log(1 / 2)

          (foo, superman)
          count(foo) = 2
          count(superman) = 1
          count(foo, superman) = 1
          val log((1 + 1) / 2) = log(2 / 2) = log(1) = 0

          (foo, spiderman)
          count(foo) = 2
          count(spiderman) = 0
          count(foo, spiderman) = 0
          val = log(1 / 2)

          (bar, batman)
          count(foo) = 2
          count(spiderman) = 0
          count(foo, spiderman) = 0
          val = log(1 / 2)

          (bar, superman)
          count(bar) = 2
          count(superman) = 1
          count(bar, superman) = 0
          val = log(1 / 2)

          (bar, spiderman)
          count(bar) = 2
          count(spiderman) = 0
          count(bar, spiderman) = 0
          val = log(1 / 2)

          (batman, superman)
          count(batman) = 3
          count(superman) = 1
          count(batman, superman) = 2
          val = log((2 + 1) / 3) = log(1) = 0

          (batman, spiderman)
          count(batman) = 3
          count(spiderman) = 0
          count(batman, spiderman) = 0
          val = log(1 / 3)

          (superman, spiderman)
          count(superman) = 1
          count(spiderman) = 0
          count(superman, spiderman) = 0
          val = log(1 / 1) = 0
        */

        TopicCoherenceEvaluator evaluator{word_counts};

        double actual = evaluator.evaluate({"foo", "bar", "batman", "superman", "spiderman"});
        double expected = log((double) 3 / 2) + 5 * log((double) 1 / 2) + log((double) 1 / 3);
        expect_true(is_equal(actual, expected));
      }
    }

  } // namespace test
} // namespace tomer
