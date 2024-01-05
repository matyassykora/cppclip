#include "CppClip.hpp"
#include <iostream>
#include <vector>
#include <string>

const std::string VERSION = "1.0";

int main(int argc, char *argv[]) {
  CppClip::ArgumentParser input("main");

  input.addDescription("Test programDescription");
  input.addEpilogue("stuff and things");
  input.add("subnetmask").help("The subnet mask"); // an optional positional argument with help
  input.add("mask").nargs(1);
  input.add("ip").nargs(2); // 2 non-optional positional arguments

  input.add("-s").help("FDSA");
  // input.add("args").nargs(3);
  input.add("-v", "--version").help("show version and exit");
  input.add("-h", "--help").help("print help and exit"); // short & long form

  try {
    input.parse(argc, argv);

    if (input.isSet("-h")) {
      input.printHelp();
      exit(0);
    }

    if (input.isSet("-v")) {
      std::cout << "version: " << VERSION << "\n";
      exit(0);
    }

    const std::vector<std::string> &b = input.getPositional("ip");
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
