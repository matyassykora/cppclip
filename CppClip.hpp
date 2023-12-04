#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Exception : public std::exception {
public:
  explicit Exception(const std::string &message) : m_message(message) {}

  const char *what() const noexcept override {
    return m_message.c_str();
  }

private:
  std::string m_message;
};

// TODO: split this up into multiple classes (separate the responsibilities to make testing easier & the code cleaner)
class ArgumentParser {
private:
  struct argument {
    std::string shortOpt;
    bool isSet = false;
    std::string longOpt;
    std::string positionalOpt;
    std::vector<std::string> options;
    std::string helpMessage;
    bool isPositional = false;
    bool isOptional = true;
    int nargs = 0;
    std::string metavar;
    std::vector<std::string> data;
  };

public:
  ~ArgumentParser() = default;

  explicit ArgumentParser(const std::string &programName) {
    this->programName = programName;
  };

  // FIX: needing to add arguments in reverse order
  ArgumentParser &add(const std::string &option,
                      const std::string &longOpt = "") {

    if (option.empty()) {
      throw Exception("You must add at least one argument");
    }

    currentArgumentID++;

    // both long and short
    if (!longOpt.empty()) {
      if (longOpt.find("--") == std::string::npos) {
        throw Exception("Long option must start with --");
      }
      if (option.find("--") != std::string::npos) {
        throw Exception("When setting both options, the first option must not start with --");
      }
      argumentMap[currentArgumentID].shortOpt = option;
      argumentMap[currentArgumentID].longOpt = longOpt;
      return *this;
    }

    if (isPositionalOpt(option)) {
      argumentMap[currentArgumentID].positionalOpt = option;
      argumentMap[currentArgumentID].isPositional = true;
      argumentMap[currentArgumentID].nargs = 1;
      return *this;
    }

    // only long
    if (option.find("--") != std::string::npos) {
      argumentMap[currentArgumentID].longOpt = option;
      return *this;
    }



    // only short
    if (!option.empty()) {
      argumentMap[currentArgumentID].shortOpt = option;
      return *this;
    }

    throw Exception("Something went wrong when adding an argument");
  }

  ArgumentParser &help(const std::string &message) {
    if (message.empty()) {
      return *this;
    }
    getArgument(currentArgumentID).helpMessage = message;
    return *this;
  }

  ArgumentParser &nargs(int nargs) {
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
  static bool isPositionalOpt(const std::string &option) {
    return option.at(0) != '-';
  }

  void addDescription(const std::string &description) {
    if (description.empty()) { return; }
    this->programDescription = description;
  }

  void addEpilogue(const std::string &epilogue) { this->programEpilogue = epilogue; }

  void parse(const int &argc, char **argv) {
    for (int i = 1; i < argc; i++) {
      this->args.emplace_back(argv[i]);
    }
    for (const auto &arg: args) {
      for (const char &c: arg) {
        for (auto &pair: argumentMap) {
          if (pair.second.shortOpt.size() <= 1) {
            continue;
          }
          if (pair.second.shortOpt.at(1) == c) {
            pair.second.isSet = true;
          }
        }
      }
    }
  }

  std::vector<std::string> getPositional(const std::string &option) {
    std::vector<std::string> vec;
    std::vector<std::string> all = getAllPositional();
    for (const auto &pair: argumentMap) {
      if (!pair.second.isPositional) {
        continue;
      }
      if (option != pair.second.positionalOpt) {
        continue;
      }
      for (int j = 0; j < pair.second.nargs; j++) {
        if (positionalIndex >= all.size() && pair.second.isOptional) {
          continue;
        }
        if (positionalIndex >= all.size() && !pair.second.isOptional) {
          throw Exception("Not enough arguments to " + pair.second.positionalOpt);
        }
        vec.push_back(all.at(positionalIndex));
        positionalIndex++;
      }
    }

    return vec;
  }

  bool existsInMap(const std::string &option) {
    for (const auto &pair: argumentMap) {
      if (option == pair.second.shortOpt || option == pair.second.longOpt || option == pair.second.positionalOpt) {
        return true;
      }
    }
    return false;
  }

  bool isSet(const std::string &option) {
    auto it = std::find_if(argumentMap.begin(), argumentMap.end(), [&](const auto &pair) {
      return option.size() == pair.second.shortOpt.size() && option == pair.second.shortOpt && pair.second.isSet;
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
    for (const auto &pair: argumentMap) {
      if (pair.second.shortOpt.empty()) {
        continue;
      }
      for (const char &c: pair.second.shortOpt) {
        if (c == '-') {
          continue;
        }
        std::cout << c;
      }
    }
    std::cout << "] ";
  }

  void printLongOptions() {
    for (const auto &pair: argumentMap) {
      if (pair.second.shortOpt.empty() && !pair.second.longOpt.empty()) {
        std::cout << "[" << pair.second.longOpt << "] ";
      }
    }
  }

  void printPositionalOptions() {
    for (const auto &pair: argumentMap) {
      if (pair.second.positionalOpt.empty()) {
        continue;
      }
      if (pair.second.isOptional) {
        std::cout << "[" << pair.second.positionalOpt << "] ";
        continue;
      }
      std::cout << pair.second.positionalOpt << " ";
    }
  }

  void printProgramDescription() {
    if (!this->programDescription.empty()) {
      std::cout << '\n' << programDescription << '\n';
    }
  }

  void printOptions() {
    for (const auto &pair: argumentMap) {
      std::stringstream optionStream;
      std::stringstream helpStream;
      optionStream << "  " << pair.second.shortOpt;
      if (!pair.second.longOpt.empty()) {
        optionStream << ", " << pair.second.longOpt;
      }

      if (pair.second.isPositional) {
        continue;
      }
      std::cout << std::setw(30) << std::left << optionStream.str()
                << std::right << pair.second.helpMessage << "\n";
    }
  }

  void printPositionalArguments() {
    for (const auto &pair: argumentMap) {
      std::stringstream positional;
      if (!pair.second.isPositional) {
        continue;
      }
      if (pair.second.isOptional) {
        positional << "  [" << pair.second.positionalOpt << "]";
      } else {
        positional << "  " << pair.second.positionalOpt;
      }
      positional << " (" << pair.second.nargs
                 << ')';
      std::cout << std::setw(30) << std::left << positional.str() << std::right
                << pair.second.helpMessage << '\n';
    }
  }

  void printProgramEpilogue() {
    if (!this->programEpilogue.empty()) {
      std::cout << '\n' << this->programEpilogue << '\n';
    }
  }

  bool argsEmpty() { return args.empty(); }

  // TODO: make sure arguments get correctly consumed after running this
  std::vector<std::string> getArgsAfter(const std::string &option) {
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

private:
  int mapIDFromOpt(const std::string &option) {
    auto it = std::find_if(argumentMap.begin(), argumentMap.end(), [&](const auto &pair) {
      return pair.second.shortOpt == option;
    });
    if (it != argumentMap.end()) {
      return it->first;
    }
    return -1;
  }

  std::vector<std::string> getAllPositional() {
    std::vector<std::string> vec;
    for (auto i: args) {
      if (i.front() != '-') {
        vec.push_back(i);
      }
    }
    return vec;
  }

private:


  int positionalIndex = 0;

  std::vector<std::string> args;
  std::string programDescription;
  std::string programEpilogue;

  std::string programName;
  int currentArgumentID = -1;
  std::unordered_map<int, argument> argumentMap;
};
