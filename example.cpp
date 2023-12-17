/*
#include "CppClip.hpp"
#include <iostream>

const std::string VERSION = "1.0";

int main(int argc, char *argv[]) {
  CppClip::ArgumentParser input("main");

  input.addDescription("Test programDescription");
  input.addEpilogue("stuff and things");
  input.add("subnetmask").help("The subnet mask");
  input.add("mask").nargs(1);
  input.add("ip").nargs(1);

  input.add("-s").help("FDSA");
  // input.add("args").nargs(3);
  input.add("-v", "--version").help("show version and exit");
  input.add("-h", "--help").help("print help and exit");

  try {
    input.parse(argc, argv);

    // auto vec = input.getArgsAfter("-argumentMap");
    // for (const auto &i : vec) {
    //   std::cout << i << "\n";
    // }

    if (input.isSet("-h") || input.isSet("--help")) {
      input.printHelp();
      exit(0);
    }

    if (input.isSet("-v") || input.isSet("--version")) {
      std::cout << "version: " << VERSION << "\n";
      exit(0);
    }

    auto b = input.getPositional("ip");
    auto c = input.getPositional("mask");
    auto d = input.getPositional("subnetmask");

    for (const auto &i : b) {
      std::cout << i << '\n';
    }

    for (const auto &i : c) {
      std::cout << i << '\n';
    }

    for (const auto &i : d) {
      std::cout << i << '\n';
    }
  } catch (const std::exception &e) {
    input.printHelp();
    std::cerr << "\nError: " << e.what() << '\n';
    exit(1);
  }
}
*/
