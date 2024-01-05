# cppclip

> C++ cli parser

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A small C++ library to simplify parsing arguments

## Usage
### Include in your app
```cpp
#include "CppClip.hpp"
```

### Instantiate the parser
```cpp
CppClip::ArgumentParser parser("program");
```

### Add a description
```cpp
parser.addDescription("Program description");
```

### Add an epilogue
```cpp
parser.addEpilogue("Epilogue");
```

### Add arguments
The parser uses method chaining to specify parameters for arguments
**Arguments must be added in reverse order!** (bottom to top gets processed left to right)

#### Non-optional positional arguments
```cpp
parser.add("ip").nargs(2);
```

#### Long & short argument form
```cpp
parser.add("-v", "--version").help("show version and exit");
parser.add("-h", "--help").help("Show help and exit");
```

#### Adding help to an argument
```cpp
parser.add("-s").help("something");
```

#### Add an optional positional argument
```cpp
parser.add("number");
```


### Processing arguments
Exception handling is done using a try catch block
```cpp
try {
    ...
} catch (std::exception &e) {
    parser.printHelp();
    std::cerr << "\nError: " << e.what() << '\n';
    exit(1);
}
```

#### Parsing user input
```cpp
parser.parse(argc, argv);
```

#### Checking if an option is set
```cpp
// The long option is checked automatically
if (parser.isSet("-h")) {
    parser.printHelp();
    exit(0);
}
```

#### Extracting positional arguments
```cpp
const std::vector<std::string> &ip = parser.getPositional("ip");
```

### Missing features
- Ability to add multiple optional positional arguments
