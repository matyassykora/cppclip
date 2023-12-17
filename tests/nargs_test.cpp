#include <gtest/gtest.h>
#include "CppClip.hpp"

using namespace CppClip;

TEST(ArgumentParserTest, test_nargs_throws_exception_with_non_positive_integer) {
  ArgumentParser parser("test");

  parser.add("positional").nargs(2);

  EXPECT_THROW(parser.nargs(0), Exception);
  EXPECT_THROW(parser.nargs(-1), Exception);
}

TEST(ArgumentParserTest, test_nargs_does_not_throw_exception_with_positive_integer) {
  ArgumentParser parser("test");

  parser.add("positional").nargs(2);

  EXPECT_NO_THROW(parser.nargs(1));
  EXPECT_NO_THROW(parser.nargs(2));
}
