#include <testthat.h>

#include "def.h"
#include "type_sequence_builder.h"
#include "type_sequence_container.h"
#include "type_sequence.h"

namespace tomer {
  namespace test {

    const Document DOCUMENT{"hello", "world", "hello"};
    const Corpus CORPUS{DOCUMENT, DOCUMENT, DOCUMENT};
    const Alphabet ALPHABET{{{"hello", 1}}};

    context("TypeSequenceBuilder") {

      test_that("Default TypeSequenceBuilder builds document correctly") {
        TypeSequenceBuilder builder;
        builder.add(DOCUMENT);

        TypeSequenceContainer container = builder.get_data();
        expect_true(container.size() == 1);

        TypeSequence sequence = container.at(0);
        expect_true(sequence.size() == 3);
        expect_true(sequence.length() == sequence.size());

        expect_error_as(sequence.at(-1), std::out_of_range);
        expect_true(sequence.at(0) == 0);
        expect_true(sequence.at(1) == 1);
        expect_true(sequence.at(2) == 0);
        expect_error_as(sequence.at(3), std::out_of_range);

        expect_error_as(sequence.token_at(-1), std::out_of_range);
        expect_true(sequence.token_at(0) == "hello");
        expect_true(sequence.token_at(1) == "world");
        expect_true(sequence.token_at(2) == "hello");
        expect_error_as(sequence.token_at(3), std::out_of_range);
      }

      test_that("Default TypeSequenceBuilder builds corpus correctly") {
        TypeSequenceBuilder builder;
        builder.add(CORPUS);

        TypeSequenceContainer container = builder.get_data();
        expect_true(container.size() == 3);

        for (unsigned i = 0; i < container.size(); ++i) {
          TypeSequence sequence = container.at(i);
          expect_true(sequence.size() == 3);

          expect_error_as(sequence.at(-1), std::out_of_range);
          expect_true(sequence.at(0) == 0);
          expect_true(sequence.at(1) == 1);
          expect_true(sequence.at(2) == 0);
          expect_error_as(sequence.at(3), std::out_of_range);

          expect_error_as(sequence.token_at(-1), std::out_of_range);
          expect_true(sequence.token_at(0) == "hello");
          expect_true(sequence.token_at(1) == "world");
          expect_true(sequence.token_at(2) == "hello");
          expect_error_as(sequence.token_at(3), std::out_of_range);
        }
      }

      test_that("TypeSequenceBuilder with non-fixed alphabet builds corpus correctly") {
        TypeSequenceBuilder builder{ALPHABET, false};
        builder.add(CORPUS);

        TypeSequenceContainer container = builder.get_data();
        expect_true(container.size() == 3);

        for (unsigned i = 0; i < container.size(); ++i) {
          TypeSequence sequence = container.at(i);
          expect_true(sequence.size() == 3);

          expect_error_as(sequence.at(-1), std::out_of_range);
          expect_true(sequence.at(0) == 1);
          expect_true(sequence.at(1) == 2);
          expect_true(sequence.at(2) == 1);
          expect_error_as(sequence.at(3), std::out_of_range);

          expect_error_as(sequence.token_at(-1), std::out_of_range);
          expect_true(sequence.token_at(0) == "hello");
          expect_true(sequence.token_at(1) == "world");
          expect_true(sequence.token_at(2) == "hello");
          expect_error_as(sequence.token_at(3), std::out_of_range);
        }
      }

      test_that("TypeSequenceBuilder with fixed alphabet builds corpus correctly") {
        TypeSequenceBuilder builder{ALPHABET, true};
        builder.add(CORPUS);

        TypeSequenceContainer container = builder.get_data();
        expect_true(container.size() == 3);

        for (unsigned i = 0; i < container.size(); ++i) {
          TypeSequence sequence = container.at(i);
          expect_true(sequence.size() == 2);

          expect_error_as(sequence.at(-1), std::out_of_range);
          expect_true(sequence.at(0) == 1);
          expect_true(sequence.at(1) == 1);
          expect_error_as(sequence.at(2), std::out_of_range);

          expect_error_as(sequence.token_at(-1), std::out_of_range);
          expect_true(sequence.token_at(0) == "hello");
          expect_true(sequence.token_at(1) == "hello");
          expect_error_as(sequence.token_at(2), std::out_of_range);
        }
      }
    }

  } // namespace test
} // namespace tomer
