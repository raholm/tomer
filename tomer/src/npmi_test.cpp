#include <testthat.h>

#include "test_helper.h"
#include "npmi.h"

namespace tomer {
  namespace test {

    context("npmi") {
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

      test_that("TopicWordIndexRelation works properly") {
        TopicWordIndexRelation relation(0);

        WordIndex idata = 1;
        Vector<WordIndex> vdata{0, 1, UNOBSERVED_WORDINDEX, 2, 1};

        expect_false(relation.is_related_to(0));
        expect_false(relation.is_related_to(1));
        expect_false(relation.is_related_to(2));
        expect_false(relation.is_related_to(UNOBSERVED_WORDINDEX));

        relation.update(idata);
        relation.update(vdata);

        expect_false(relation.is_related_to(0));
        expect_true(relation.is_related_to(1));
        expect_true(relation.is_related_to(2));
        expect_false(relation.is_related_to(UNOBSERVED_WORDINDEX));

        TopicWordIndexRelation relation2(0, idata);
        expect_false(relation2.is_related_to(0));
        expect_true(relation2.is_related_to(1));

        TopicWordIndexRelation relation3(0, vdata);
        expect_false(relation3.is_related_to(0));
        expect_true(relation3.is_related_to(1));
        expect_true(relation3.is_related_to(2));
        expect_false(relation3.is_related_to(UNOBSERVED_WORDINDEX));
      }

      test_that("TopicWordIndexRelationMap works properly") {
        TopicWordIndexRelationMap relations;

        relations.update(0, 1);
        relations.update(0, {1, 2, 3, 0, 1});
        relations.update(1, 0);
        relations.update(2, {1, 0, 2});

        expect_false(relations.is_related_to(0, 0));
        expect_true(relations.is_related_to(0, 1));
        expect_true(relations.is_related_to(0, 2));
        expect_true(relations.is_related_to(0, 3));

        expect_true(relations.is_related_to(1, 0));
        expect_false(relations.is_related_to(1, 1));
        expect_false(relations.is_related_to(1, 2));
        expect_false(relations.is_related_to(1, 3));

        expect_true(relations.is_related_to(2, 0));
        expect_true(relations.is_related_to(2, 1));
        expect_false(relations.is_related_to(2, 2));
        expect_false(relations.is_related_to(2, 3));
      }

      test_that("WordCount works properly") {
        WordToIndexTransformer transformer;
        transformer.update({"foo", "bar", "batman", "superman"});

        TopicWordIndexRelationMap relations;
        relations.update(transformer.transform("foo"), transformer.transform("bar"));
        relations.update(transformer.transform("batman"), transformer.transform("superman"));
        relations.update(transformer.transform("superman"), transformer.transform(Vector<Word>{"foo", "bar"}));

        WordCount word_counts{transformer, relations};

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

      test_that("NormalisedPointwiseMutualInformationEvaluator works properly") {

      }
    }

  } // namespace test
} // namespace tomer
