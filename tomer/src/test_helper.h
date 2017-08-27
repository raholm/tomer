#ifndef TOMER_TEST_HELPER_H_
#define TOMER_TEST_HELPER_H_

#include <testthat.h>

#include "def.h"

namespace tomer {
  namespace test {

    template<typename T>
    void check_equality(const Vector<T>& v1, const Vector<T>& v2) {
      expect_true(v1.size() == v2.size());

      for (unsigned i = 0; i < v1.size(); ++i)
        expect_true(v1.at(i) == v2.at(i));
    }

  } // namespace test
} // namespace tomer

#endif // TOMER_TEST_HELPER_H_
