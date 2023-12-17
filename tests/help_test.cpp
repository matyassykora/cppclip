#include <gtest/gtest.h>
#include "CppClip.hpp"

using namespace CppClip;

// help message is set when non-empty
TEST(ArgumentParserTest, test_help_message_is_set_for_argument) {
  ArgumentParser parser("program");

  parser.add("arg").help("This is the help message");

  EXPECT_EQ(parser.getArgument("arg").helpMessage, "This is the help message");
}

// help message is not set when empty
TEST(ArgumentParserTest, test_help_message_not_set_when_empty) {
  ArgumentParser parser("program");

  parser.add("-a").help("");

  ASSERT_EQ(parser.getArgument("-a").helpMessage, "");
}

// help message is set for the correct argument
TEST(ArgumentParserTest, test_help_message_set_for_correct_argument) {
  ArgumentParser parser("program");

  parser.add("-a").help("This is the help message");
  parser.add("-b").help("Another help message");

  ASSERT_EQ(parser.getArgument("-a").helpMessage, "This is the help message");
  ASSERT_EQ(parser.getArgument("-b").helpMessage, "Another help message");
}

TEST(ArgumentParserTest, test_help_message_not_set_for_specific_argument) {
  ArgumentParser parser("program");

  parser.add("-a");
  parser.add("--b").help("Help message for -b option");

  EXPECT_EQ(parser.getArgument("-a").helpMessage, "");
  EXPECT_EQ(parser.getArgument("--b").helpMessage, "Help message for -b option");
}
