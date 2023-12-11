#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Exception : public std::exception {
public:
  explicit Exception(std::string_view message) : m_message(message) {}

  const char *what() const noexcept override { return m_message.c_str(); }

private:
  std::string m_message;
};

struct argument {
  std::string shortOpt;
  std::string longOpt;
  std::string positionalOpt;
  std::string helpMessage;
  int nargs = 0;
  bool isSet = false;
  bool isPositional = false;
  bool isOptional = true;
};

class ArgumentParser {

public:
  ~ArgumentParser() = default;

  explicit ArgumentParser(std::string_view programName) {
    this->programName = programName;
  };

  // FIX: needing to add arguments in reverse order
  ArgumentParser &add(const std::string_view option,
                      const std::string_view longOpt = "") {

    if (option.empty()) {
      throw Exception("You must add at least one argument");
    }

    currentArgumentID++;
    auto &argument = argumentMap[currentArgumentID];

    // both long and short
    if (!longOpt.empty()) {
      if (longOpt.find("--") == std::string::npos) {
        throw Exception("Long option must start with --");
      }
      if (option.find("--") != std::string::npos) {
        throw Exception("When setting both options, the first option must not "
                        "start with --");
      }
      argument.shortOpt = option;
      argument.longOpt = longOpt;
      return *this;
    }

    if (isPositionalOpt(option)) {
      argument.positionalOpt = option;
      argument.isPositional = true;
      argument.nargs = 1;
      return *this;
    }

    // only long
    if (option.find("--") != std::string::npos) {
      argument.longOpt = option;
      return *this;
    }

    // only short
    if (!option.empty()) {
      argument.shortOpt = option;
      return *this;
    }

    throw Exception("Something went wrong when adding an argument");
  }

  ArgumentParser &help(std::string_view message) {
    if (message.empty()) {
      return *this;
    }
    getArgument(currentArgumentID).helpMessage = message;
    return *this;
  }

  ArgumentParser &nargs(const int &nargs) {
    if (nargs <= 0) {
      throw Exception("Number of arguments must be >0");
    }
    auto &arg = getArgument(currentArgumentID);
    arg.nargs = nargs;
    arg.isPositional = true;
    arg.isOptional = false;
    return *this;
  }

  // TODO: improve checking whether the option is positional
  static bool isPositionalOpt(std::string_view option) {
    return option.at(0) != '-';
  }

  void addDescription(std::string_view description) {
    if (description.empty()) {
      return;
    }
    this->programDescription = description;
  }

  void addEpilogue(std::string_view epilogue) {
    this->programEpilogue = epilogue;
  }

  void parse(const int &argc, char **argv) {
    for (int i = 1; i < argc; i++) {
      this->args.emplace_back(argv[i]);
    }

    for (const auto &arg : this->args) {
      if (arg.find("--") != std::string::npos) {
        checkUnrecognized(arg);
        findLong(arg);
        continue;
      }
      parseShort(arg);
    }
  }

  std::vector<std::string> getPositional(const std::string_view option) {
    std::vector<std::string> vec;
    std::vector<std::string> all = getAllPositional();
    for (const auto &[key, opt] : argumentMap) {
      if (!opt.isPositional) {
        continue;
      }
      if (option != opt.positionalOpt) {
        continue;
      }
      for (int j = 0; j < opt.nargs; j++) {
        if (positionalIndex >= all.size() && opt.isOptional) {
          continue;
        }
        if (positionalIndex >= all.size() && !opt.isOptional) {
          throw Exception("Not enough arguments to " + opt.positionalOpt);
        }
        vec.push_back(all.at(positionalIndex));
        positionalIndex++;
      }
    }

    return vec;
  }

  bool existsInMap(const std::string_view option) {
    for (const auto &[key, opt] : argumentMap) {
      if (option == opt.shortOpt || option == opt.longOpt ||
          option == opt.positionalOpt) {
        return true;
      }
    }
    return false;
  }

  bool isSet(const std::string_view option) {
    auto it = std::find_if(
        argumentMap.begin(), argumentMap.end(), [&](const auto &pair) {
          return option.size() == pair.second.shortOpt.size() &&
                 option == pair.second.shortOpt && pair.second.isSet;
        });
    return it != argumentMap.end();
  }

  void printHelp() {
    std::cout << "Usage: " << programName << " ";
    printShortOptions();
    printLongOptions();
    printPositionalOptions();
    std::cout << '\n';

    printProgramDescription();

    std::cout << "\nOptions:\n";
    printOptions();

    std::cout << "\nPositional arguments:\n";
    printPositionalArguments();

    printProgramEpilogue();
  }

  void printShortOptions() {
    if (argumentMap.empty()) {
      return;
    }
    std::cout << "[-";
    for (const auto &pair : argumentMap) {
      if (pair.second.shortOpt.empty()) {
        continue;
      }
      for (const char &c : pair.second.shortOpt) {
        if (c == '-') {
          continue;
        }
        std::cout << c;
      }
    }
    std::cout << "] ";
  }

  void printLongOptions() {
    for (const auto &[key, opt] : argumentMap) {
      if (opt.shortOpt.empty() && !opt.longOpt.empty()) {
        std::cout << "[" << opt.longOpt << "] ";
      }
    }
  }

  void printPositionalOptions() {
    for (const auto &[key, opt] : argumentMap) {
      if (opt.positionalOpt.empty()) {
        continue;
      }
      if (opt.isOptional) {
        std::cout << "[" << opt.positionalOpt << "] ";
        continue;
      }
      std::cout << opt.positionalOpt << " ";
    }
  }

  void printProgramDescription() {
    if (!this->programDescription.empty()) {
      std::cout << '\n' << programDescription << '\n';
    }
  }

  void printOptions() {
    for (const auto &[key, opt] : argumentMap) {
      std::stringstream optionStream;
      std::stringstream helpStream;
      optionStream << "  " << opt.shortOpt;
      if (!opt.longOpt.empty()) {
        optionStream << ", " << opt.longOpt;
      }

      if (opt.isPositional) {
        continue;
      }
      std::cout << std::setw(30) << std::left << optionStream.str()
                << std::right << opt.helpMessage << "\n";
    }
  }

  void printPositionalArguments() {
    for (const auto &[key, opt] : argumentMap) {
      std::stringstream positional;
      if (!opt.isPositional) {
        continue;
      }
      if (opt.isOptional) {
        positional << "  [" << opt.positionalOpt << "]";
      } else {
        positional << "  " << opt.positionalOpt;
      }
      positional << " (" << opt.nargs << ')';
      std::cout << std::setw(30) << std::left << positional.str() << std::right
                << opt.helpMessage << '\n';
    }
  }

  void printProgramEpilogue() {
    if (!this->programEpilogue.empty()) {
      std::cout << '\n' << this->programEpilogue << '\n';
    }
  }

  bool argsEmpty() { return args.empty(); }

  // TODO: make sure arguments get correctly consumed after running this
  std::vector<std::string> getArgsAfter(const std::string_view option) {
    int id = mapIDFromOpt(option);
    if (id == -1) {
      throw Exception("Thís argument doesn't exist");
    }
    std::vector<std::string> vec;
    std::vector<std::string>::const_iterator itr;

    const auto &map = argumentMap[id];
    int nargs = map.nargs;

    itr = std::find(this->args.begin(), this->args.end(), option);
    for (int i = 0; i < nargs; i++) {
      if (itr != this->args.end() && ++itr != this->args.end()) {
        vec.push_back(*itr);
      }
    }
    return vec;
  }

  argument &getArgument(int id) {
    if (id < 0 || id >= argumentMap.size()) {
      throw Exception("ID out of range");
    }
    if (argumentMap.count(id) == 0) {
      throw Exception("ID does not exist");
    }
    return argumentMap.at(id);
  }

  void allowUnrecognized() { unrecognizedAllowed = true; }

private:
  int mapIDFromOpt(const std::string_view option) {
    auto it = std::find_if(
        argumentMap.begin(), argumentMap.end(),
        [&](const auto &pair) { return pair.second.shortOpt == option; });

    if (it != argumentMap.end()) {
      return it->first;
    }
    return -1;
  }

  const std::vector<std::string> getAllPositional() {
    std::vector<std::string> vec;
    for (const auto &i : args) {
      if (i.front() != '-') {
        vec.push_back(i);
      }
    }
    return vec;
  }

  void findLong(const std::string &arg) {
    for (auto &[key, opt] : this->argumentMap) {
      if (opt.longOpt == arg) {
        opt.isSet = true;
      }
    }
  }

  void parseShort(const std::string &arg) {
    if (arg.front() != '-') {
      return;
    }
    for (const char &c : arg) {
      if (c != '-') {
        std::string ab(1, '-');
        ab.append(1, c);
        checkUnrecognized(std::string(1, '-').append(1, c));
      }
      for (auto &[key, opt] : this->argumentMap) {
        if (opt.shortOpt.size() <= 1) {
          continue;
        }
        if (opt.shortOpt.at(1) == c) {
          opt.isSet = true;
        }
      }
    }
  }

  void checkUnrecognized(const std::string &arg) {
    if (!existsInMap(arg)) {
      this->unrecognizedFound = true;
    }
    if (this->unrecognizedFound && !this->unrecognizedAllowed) {
      throw Exception("Unrecognized option: " + arg);
    }
  }

private:
  bool unrecognizedFound = false;
  bool unrecognizedAllowed = false;
  int positionalIndex = 0;

  std::vector<std::string> args;
  std::string programDescription;
  std::string programEpilogue;

  std::string programName;
  int currentArgumentID = -1;
  std::unordered_map<int, argument> argumentMap;
};
