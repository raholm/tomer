#include <testthat.h>

#include "alphabet.h"

namespace tomer {
  namespace test {

    const Alphabet ALPHABET{{
        {"hello", 1}, {"world", 2}
      }};

    static void has(const Alphabet& alphabet,
                    const Alphabet::Token& token,
                    const Alphabet::Type& type) {
      expect_true(alphabet.has(token));
      expect_true(alphabet.has(type));

      expect_true(alphabet.at(token) == type);
      expect_true(alphabet.at(type) == token);
    }

    context("Alphabet") {
      test_that("alphabet copies correctly") {
        Alphabet a1{ALPHABET};
        has(a1, "hello", 1);
        has(a1, "world", 2);
        expect_true(a1.size() == 2);

        Alphabet a2;
        a2 = ALPHABET;
        has(a2, "hello", 1);
        has(a2, "world", 2);
        expect_true(a2.size() == 2);

        Alphabet a3{std::move(a1)};
        has(a3, "hello", 1);
        has(a3, "world", 2);
        expect_true(a3.size() == 2);

        Alphabet a4;
        a4 = std::move(a2);
        has(a4, "hello", 1);
        has(a4, "world", 2);
        expect_true(a4.size() == 2);
      }

      test_that("alphabet has types") {
        expect_true(ALPHABET.has(1));
        expect_true(ALPHABET.has(2));

        expect_false(ALPHABET.has(0));
        expect_false(ALPHABET.has(3));
      }

      test_that("alphabet has tokens") {
        expect_true(ALPHABET.has("hello"));
        expect_true(ALPHABET.has("world"));

        expect_false(ALPHABET.has("Hello"));
        expect_false(ALPHABET.has("hi"));
      }

      test_that("alphabet at types") {
        expect_true(ALPHABET.at(1) == "hello");
        expect_true(ALPHABET.at(2) == "world");

        expect_error_as(ALPHABET.at("hi"), std::out_of_range);
      }

      test_that("alphabet at tokens") {
        expect_true(ALPHABET.at("hello") == 1);
        expect_true(ALPHABET.at("world") == 2);

        expect_error_as(ALPHABET.at("Hello"), std::out_of_range);
      }

      test_that("alphabet size is correct") {
        expect_true(ALPHABET.size() == 2);
      }

      test_that("alphabet adds correctly") {
        Alphabet alphabet{ALPHABET};
        expect_true(alphabet.add("hi") == 3);
        expect_true(alphabet.size() == 3);

        has(alphabet, "hi", 3);
      }

      test_that("alphabet adds duplicates correctly") {
        Alphabet alphabet;

        expect_true(alphabet.add("hi") == 0);
        has(alphabet, "hi", 0);

        expect_true(alphabet.add("hi") == 0);
        has(alphabet, "hi", 0);

        expect_true(alphabet.size() == 1);
      }
    }

  } // namespace test
} // namespace tomer
