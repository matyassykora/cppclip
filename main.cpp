/*
#include "CppClip.hpp"
#include <cstdlib>
#include <iostream>
 const std::string VERSION = "1.0";
 */
#include <gtest/gtest.h>

// TODO: check this out:
// https://www.codeproject.com/Tips/5261900/Cplusplus-Lightweight-Parsing-Command-Line-Argumen

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();

/*
  ArgumentParser input("main");

  input.addDescription("Test programDescription");
  input.addEpilogue("stuff and things");
  input.add("ip").nargs(1);
  input.add("mask");
  input.add("subnetmask");
  // input.add("-o", "--option").help("P");
  // input.add("-argumentMap", "--anything").help("ASDF").nargs(2);
  // input.add("-s", "--something").help("FDSA");
  // input.add("args").nargs(3);
  input.add("-v").help("show version and exit");
  input.add("-h", "--help").help("print help and exit");

  input.parse(argc, argv);

  // auto vec = input.getArgsAfter("-argumentMap");
  // for (auto i : vec) {
  //   std::cout << i << "\n";
  // }

  if (input.argsEmpty() || input.isSet("-h")) {
    input.printHelp();
    exit(0);
  }

  if (input.isSet("-v")) {
    std::cout << "version: " << VERSION << "\n";
    exit(0);
  }

  auto b = input.getPositional("ip");
  for (auto i: b) {
    std::cout << i << "\n";
  }

  auto c = input.getPositional("mask");
  for (auto i: c) {
    std::cout << i << "\n";
  }

  auto d = input.getPositional("subnetmask");
  for (auto i: d) {
    std::cout << i << "\n";
  }
*/
}
