
#include "CppClip.hpp"
#include <cstdlib>
#include <iostream>

const std::string VERSION = "1.0";

#include <gtest/gtest.h>

// TODO: check this out:
// https://www.codeproject.com/Tips/5261900/Cplusplus-Lightweight-Parsing-Command-Line-Argumen

int main(int argc, char *argv[]) {
  /*
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
   */


  ArgumentParser input("main");

  input.addDescription("Test programDescription");
  input.addEpilogue("stuff and things");
  input.add("subnetmask");
  input.add("mask").nargs(1);
  input.add("ip").nargs(1);

  input.add("-s").help("FDSA");
  // input.add("args").nargs(3);
  input.add("-v", "--version").help("show version and exit");
  input.add("-h", "--help").help("print help and exit");

  input.parse(argc, argv);

  // auto vec = input.getArgsAfter("-argumentMap");
  // for (auto i : vec) {
  //   std::cout << i << "\n";
  // }

  if (input.isSet("-h")) {
    input.printHelp();
    exit(0);
  }

  if (input.isSet("-v")) {
    std::cout << "version: " << VERSION << "\n";
    exit(0);
  }

  try {
    auto b = input.getPositional("ip");
    auto c = input.getPositional("mask");
    auto d = input.getPositional("subnetmask");

    for (const auto &i: b) {
      std::cout << i << '\n';
    }

    for (const auto &i: c) {
      std::cout << i << '\n';
    }

    for (const auto &i: d) {
      std::cout << i << '\n';
    }
  }
  catch (const std::exception &e) {
    input.printHelp();
    std::cout << '\n' << e.what() << '\n';
    exit(1);
  }

}
