#pragma once
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class ArgumentParser {
public:
  ~ArgumentParser() = default;
  explicit ArgumentParser(const std::string &programName) {
    this->programName = programName;
  };

  // FIX: needing to add arguments in reverse?
  ArgumentParser &add(const std::string &option,
                      const std::string &longOpt = "") {
    id++;

    // positional
    if (option.at(0) != '-') {
      argumentMap[id].positionalOpt = option;
      argumentMap[id].isPositional = true;
      argumentMap[id].nargs = 1;
      return *this;
    }

    // only long
    if (option.find("--") != std::string::npos) {
      argumentMap[id].longOpt = option;
      return *this;
    }

    // both long and short
    if (!longOpt.empty()) {
      argumentMap[id].shortOpt = option;
      argumentMap[id].longOpt = longOpt;
      return *this;
    }

    // only short
    if (!option.empty()) {
      argumentMap[id].shortOpt = option;
    }

    return *this;
  }

  ArgumentParser &help(const std::string &message) {
    argumentMap[id].helpMessage = message;
    return *this;
  }

  ArgumentParser &nargs(int nargs) {
    argumentMap[id].nargs = nargs;
    argumentMap[id].isPositional = true;
    return *this;
  }

  void addDescription(const std::string &description) {
    this->programDescription = description;
  }

  void addEpilogue(const std::string &epilogue) { this->programEpilogue = epilogue; }

  void parse(const int &argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
      this->args.emplace_back(argv[i]);
    }
    for (const auto &arg : args) {
      for (const char &c : arg) {
        for (auto &pair : argumentMap) {
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
    for (const auto& i : argumentMap) {
      if (!i.second.isPositional) {
        continue;
      }
      if (option != i.second.positionalOpt) {
        continue;
      }
      for (int j = 0; j < i.second.nargs; j++) {
        if (positionalIndex >= all.size()) {
          std::cout << "Not enough arguments to \"" << i.second.positionalOpt
                    << "\"!\n";
          exit(1);
        }
        vec.push_back(all.at(positionalIndex));
        positionalIndex++;
      }
    }

    return vec;
  }

  bool isSet(const std::string& option) {
    for (const auto& pair : argumentMap) {
      if (option.size() != pair.second.shortOpt.size()) {
        continue;
      }
      if (option == pair.second.shortOpt && pair.second.isSet) {
        return true;
      }
    }
    return false;
  }

  void printHelp() {
    std::cout << "Usage: " << programName << " ";

    // print short options
    for (const auto& i : argumentMap) {
      if (i.second.shortOpt.empty()) {
        continue;
      }
      std::cout << "[" << i.second.shortOpt << "] ";
    }

    // print long options
    for (const auto& i : argumentMap) {
      if (i.second.shortOpt.empty() && !i.second.longOpt.empty()) {
        std::cout << "[" << i.second.longOpt << "] ";
      }
    }

    // print positional options
    for (const auto& i : argumentMap) {
      if (i.second.positionalOpt.empty()) {
        continue;
      }
      std::cout << i.second.positionalOpt << " ";
    }

    std::cout << '\n';

    if (!this->programDescription.empty()) {
      std::cout << "\n" << programDescription << "\n";
    }

    std::cout << "\nOptions:\n";
    for (const auto &i : argumentMap) {
      std::stringstream optionStream;
      std::stringstream helpStream;
      optionStream << "  " << i.second.shortOpt;
      if (!i.second.longOpt.empty()) {
        optionStream << ", " << i.second.longOpt;
      }

      if (i.second.isPositional) {
        continue;
      }
      std::cout << std::setw(30) << std::left << optionStream.str()
                << std::right << i.second.helpMessage << "\n";
    }

    std::cout << "\nPositional arguments:\n";
    for (const auto &i : argumentMap) {
      std::stringstream positional;
      if (!i.second.isPositional) {
        continue;
      }
      positional << "  " << i.second.positionalOpt << " (" << i.second.nargs
                 << ')';
      std::cout << std::setw(30) << std::left << positional.str() << std::right
                << i.second.helpMessage << "\n";
    }

    if (!this->programEpilogue.empty()) {
      std::cout << '\n' << this->programEpilogue << "\n";
    }
  }

  bool argsEmpty() { return args.empty(); }

  // TODO: make sure arguments get correctly consumed after running this
  std::vector<std::string> getArgsAfter(const std::string &option) {
    std::vector<std::string> vec;
    std::vector<std::string>::const_iterator itr;
    int id = mapIDFromArg(option);
    const auto &map = argumentMap[id];
    int nargs = map.nargs;
    if (id == -1) {
      std::cout << "These arguments don't exist!\n";
      exit(1);
    }
    itr = std::find(this->args.begin(), this->args.end(), option);
    for (int i = 0; i < nargs; i++) {
      if (itr != this->args.end() && ++itr != this->args.end()) {
        vec.push_back(*itr);
      }
    }
    return vec;
  }

private:
  int mapIDFromArg(const std::string &j) {
    int id = -1;

    for (const std::pair<const int, argument> &i : argumentMap) {
      const std::string &opts = i.second.shortOpt;
      if (i.second.shortOpt == j) {
        id = i.first;
        return id;
      }
    }

    return id;
  }

  std::vector<std::string> getAllPositional() {
    std::vector<std::string> vec;
    for (auto i : args) {
      if (i[0] != '-') {
        vec.push_back(i);
      }
    }
    return vec;
  }

private:
  struct argument {
    std::string shortOpt;
    bool isSet = false;
    std::string longOpt;
    std::string positionalOpt;
    std::vector<std::string> options;
    std::string helpMessage;
    bool isPositional = false;
    int nargs = 0;
    std::string metavar;
    std::vector<std::string> data;
  };

  int positionalIndex = 0;

  std::vector<std::string> args;
  std::string programDescription;
  std::string programEpilogue;

  std::string programName;
  int id = -1;
  std::unordered_map<int, argument> argumentMap;
};
