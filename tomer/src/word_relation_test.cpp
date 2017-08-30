#include <testthat.h>

#include "test_helper.h"
#include "word_relation.h"

namespace tomer {
  namespace test {

    context("word relation") {
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
    }

  } // namespace test
} // namespace tomer
