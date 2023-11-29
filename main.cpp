#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class ArgumentParser {
public:
  ~ArgumentParser() = default;
  ArgumentParser(const std::string &programName) {
    this->programName = programName;
  };

  template <typename... Args> ArgumentParser &add(Args... args) {
    id++;
    (a[id].options.push_back(args), ...);
    return *this;
  }

  ArgumentParser &help(const std::string &message) {
    a[id].helpMessage = message;
    return *this;
  }

  ArgumentParser &nargs(int nargs) {
    if (nargs < 1) {
      return *this;
    }
    a[id].positional = true;
    a[id].nargs = nargs;
    return *this;
  }

  ArgumentParser &metavar(const std::string &metavar) {
    a[id].metavar = metavar;
    return *this;
  }

  void addDescription(const std::string &description) {
    this->description = description;
  }

  void addEpilogue(const std::string &epilogue) { this->epilogue = epilogue; }

  void printHelp() {
    std::cout << "Usage: " << programName << " [-h] ";
    std::vector<int> positionalIDs;
    for (const std::pair<const int, argument> &i : a) {
      if (i.second.positional) {
        positionalIDs.push_back(i.first);
        std::cout << i.second.options.front();
        for (int j = 0; j < i.second.nargs; j++) {
          std::cout << ' ' << i.second.metavar;
        }
        std::cout << ' ';
        continue;
      }
      std::cout << "[" << i.second.options.front() << "] ";
    }

    if (!this->description.empty()) {
      std::cout << "\n\n" << description;
    }

    if (!positionalIDs.empty()) {
      std::cout << "\n\nPositional arguments:\n";
      for (const int &i : positionalIDs) {
        std::stringstream positionalStream;
        positionalStream << "  ";
        int k = 0;
        for (const std::string &j : a[i].options) {
          positionalStream << j;
          k++;
          if (k != a[i].options.size()) {
            positionalStream << ", ";
          }
        }
        std::cout << std::setw(30) << std::left << positionalStream.str()
                  << std::right << a[i].helpMessage << "\n";
      }

      std::cout << "\nOptional arguments:\n";
    } else {
      std::cout << "\n\nOptional arguments:\n";
    }

    std::cout << std::setw(30) << std::left << "  -h, --help" << std::left
              << "show help and exit\n";

    for (const std::pair<const int, argument> &i : a) {
      std::stringstream optionStream;
      std::stringstream helpStream;
      optionStream << "  ";
      for (int j = 0; j < i.second.options.size(); j++) {
        if (i.second.positional) {
          continue;
        }
        optionStream << i.second.options.at(j);
        if (j != i.second.options.size() - 1) {
          optionStream << ", ";
        }
      }

      if (i.second.helpMessage.empty()) {
        std::cout << std::setw(30) << std::left << optionStream.str() << "\n";
      } else {
        if (i.second.positional) {
          continue;
        }
        helpStream << i.second.helpMessage << "\n";
        std::cout << std::setw(30) << std::left << optionStream.str()
                  << std::right << helpStream.str();
      }
    }

    if (!this->epilogue.empty()) {
      std::cout << '\n' << this->epilogue << "\n";
    }
  }

  void parseArgs(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
      this->args.push_back(std::string(argv[i]));
    }
  }

  bool optionExists(const std::string &option) {
    return std::find(this->args.begin(), this->args.end(), option) !=
           this->args.end();
  }

  template <typename... Args>
  std::vector<std::string> getArgsAfter(Args... args) {
    std::vector<std::string> arguments;
    std::vector<std::string>::const_iterator iter;
    // std::vector<std::string> input = {args...};

    int id = mapIDFromArgs(args...);
    const auto &map = a[id];
    int nargs = map.nargs;
    if (id == -1) {
      std::cout << "These arguments don't exist!\n";
      exit(1);
    }

    for (const std::string &option : map.options) {
      iter = std::find(this->args.begin(), this->args.end(), option);
      for (int i = 0; i < nargs; i++) {
        if (iter != this->args.end() && ++iter != this->args.end()) {
          arguments.push_back(*iter);
        }
      }
    }

    return arguments;
  }

  bool argsEmpty() { return args.size() - 1 == 0; }

private:
  template <typename... Args> int mapIDFromArgs(Args... args) {
    int id = -1;
    std::vector<std::string> input = {args...};
    std::vector<std::string>::const_iterator iter;

    for (const std::pair<const int, argument> &i : a) {
      for (const std::string &j : input) {
        const std::vector<std::string> &opts = i.second.options;
        iter = std::find(opts.begin(), opts.end(), j);
        if (iter != opts.end()) {
          id = i.first;
        }
      }
    }

    return id;
  }

private:
  struct argument {
    std::vector<std::string> options;
    std::string helpMessage;
    bool positional = false;
    int nargs = 0;
    std::string metavar;
  };

  std::vector<std::string> args;
  std::string description = "";
  std::string epilogue = "";

  std::string programName;
  int id = -1;
  std::unordered_map<int, argument> a;
};

int main(int argc, char *argv[]) {
  ArgumentParser input("main");
  input.parseArgs(argc, argv);

  input.addDescription("Test description");
  input.addEpilogue("stuff and things");
  input.add("-o", "--option").help("PEPA");
  input.add("-a", "--anything").help("ASDF").nargs(2).metavar("ANYTHING");
  input.add("-s", "--something").help("FDSA");
  input.add("--verbose");
  input.add("-v", "--version").help("show version and exit");

  const std::vector b = input.getArgsAfter("-a", "--anything");
  for (auto c : b) {
    std::cout << c << "\n";
  }

  if (input.optionExists("-h") || input.optionExists("--help")) {
    input.printHelp();
    exit(0);
  }

  if (input.argsEmpty()) {
    input.printHelp();
    exit(0);
  }
}
