#include <testthat.h>
#include <algorithm>

#include "test_helper.h"
#include "tokenizer.h"

namespace tomer {
  namespace test {

    context("word tokenizer") {
      test_that("WordTokenizer returns the correct tokens with default delimiter") {
        WordTokenizer tokenizer;
        String text = "Hello world! What are you doing today?";
        Vector<String> texts;
        texts.push_back("Hello World");
        texts.push_back("[ foo|bar ]");
        texts.push_back("  superman   batman superman  ");

        auto tokens1 = tokenizer.transform(text);
        check_equality(tokens1, {"Hello", "world!", "What", "are", "you", "doing", "today?"});

        auto tokens2 = tokenizer.transform(texts);
        check_equality(tokens2,  {
            {"Hello", "World"}, {"[" , "foo|bar", "]"}, {"superman", "batman", "superman"}});
      }

      test_that("WordTokenizer returns the correct tokens with specified delimiter") {
        WordTokenizer tokenizer("_");
        String text = "Hello_world! What_are_you_doing_today?";
        Vector<String> texts;
        texts.push_back("Hello_World");
        texts.push_back("[ foo|bar ]");
        texts.push_back("superman_ __batman_superman");

        auto tokens1 = tokenizer.transform(text);
        check_equality(tokens1, {"Hello", "world! What", "are", "you", "doing", "today?"});

        auto tokens2 = tokenizer.transform(texts);
        check_equality(tokens2,  {
            {"Hello", "World"}, {"[ foo|bar ]"}, {"superman", " ", "batman", "superman"}});
      }
    }

    context("word index tokenizer") {
      test_that("WordIndexTokenizer returns the correct tokens with default delimiter") {
        WordIndexTokenizer tokenizer;
        String text = "Hello world! What are you doing world!";
        Vector<String> texts;
        texts.push_back("Hello World");
        texts.push_back("[ foo|bar ]");
        texts.push_back("  superman   batman superman  ");

        auto tokens1 = tokenizer.transform(text);
        check_equality(tokens1, {0, 1, 2, 3, 4, 5, 1});

        auto tokens2 = tokenizer.transform(texts);
        check_equality(tokens2,  {
            {0, 6}, {7, 8, 9}, {10, 11, 10}});
      }

      test_that("WordIndexTokenizer returns the correct tokens with specified delimiter") {
        WordIndexTokenizer tokenizer("_");
        String text = "Hello_world!_What_are_you_doing_world!";
        Vector<String> texts;
        texts.push_back("Hello_World");
        texts.push_back("[ foo|bar ]");
        texts.push_back("superman_ _batman_superman");

        auto tokens1 = tokenizer.transform(text);
        check_equality(tokens1, {0, 1, 2, 3, 4, 5, 1});

        auto tokens2 = tokenizer.transform(texts);
        check_equality(tokens2,  {
            {0, 6}, {7}, {8, 9, 10, 8}});
      }
    }

  } // namespace test
} // namespace tomer
