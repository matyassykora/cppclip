#include <gtest/gtest.h>
#include "CppClip.hpp"

using namespace CppClip;

// Adding a short option without a long option
TEST(ArgumentParserTest, test_add_short_option_without_long_option) {
  ArgumentParser parser("program");
  parser.add("-s");
  ASSERT_TRUE(parser.existsInMap("-s"));
  ASSERT_FALSE(parser.existsInMap("--long"));
}

// Adding a long option without a short option
TEST(ArgumentParserTest, test_add_long_option_without_short_option) {
  ArgumentParser parser("program");
  parser.add("--option");
  ASSERT_TRUE(parser.existsInMap("--option"));
  ASSERT_FALSE(parser.existsInMap("-o"));
}

// Adding a short option and a long option
TEST(ArgumentParserTest, test_add_short_option_and_long_option) {
  ArgumentParser parser("program");
  parser.add("-o", "--option");
  ASSERT_TRUE(parser.existsInMap("-o"));
  ASSERT_TRUE(parser.existsInMap("--option"));
}

// Adding a positional option
TEST(ArgumentParserTest, test_add_positional_option) {
  ArgumentParser parser("program");
  parser.add("positional");
  ASSERT_TRUE(parser.existsInMap("positional"));
}

// Adding an empty option
TEST(ArgumentParserTest, test_add_empty_option) {
  ArgumentParser parser("program");
  ASSERT_THROW(parser.add(""), Exception);
}

// Adding a positional option with nargs<=0
TEST(ArgumentParserTest, test_add_positional_option_with_nargs_less_than_or_equal_to_0) {
  ArgumentParser parser("program");
  ASSERT_THROW(parser.add("positional").nargs(0), Exception);
  ASSERT_THROW(parser.add("positional").nargs(-1), Exception);
}

// Adding an option with an invalid format for a long option
TEST(ArgumentParserTest, test_add_option_with_invalid_format_for_long_option) {
  ArgumentParser parser("program");
  ASSERT_THROW(parser.add("-o", "-option"), Exception);
}

// Adding an option with an invalid format for a short option
TEST(ArgumentParserTest, test_add_option_with_invalid_format_for_short_option) {
  ArgumentParser parser("program");
  ASSERT_THROW(parser.add("--short", "--s"), Exception);
}

// Adding an option with an empty long option and a non-empty short option
TEST(ArgumentParserTest, test_add_option_with_empty_long_option_and_non_empty_short_option) {
  ArgumentParser parser("program");
  parser.add("-o", "");
  ASSERT_TRUE(parser.existsInMap("-o"));
}

// Adding an option with an empty short option and a non-empty long option
TEST(ArgumentParserTest, test_add_option_with_empty_short_option_and_non_empty_long_option) {
  ArgumentParser parser("program");
  ASSERT_THROW(parser.add("", "--long"), Exception);
}
