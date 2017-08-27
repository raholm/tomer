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

      }

      test_that("WordCount works properly") {

      }

      test_that("NormalisedPointwiseMutualInformationEvaluator works properly") {

      }
    }

  } // namespace test
} // namespace tomer
