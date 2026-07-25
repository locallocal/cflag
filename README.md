# cflag

[简体中文](docs/README.zh-CN.md)

`cflag` is a C++11 header-only library for defining and parsing command-line
flags. Its template API supports every value-bearing C++11 fundamental type,
fixed-width integer aliases, and strings out of the box, while
`flag_traits<T>` makes custom flag types possible.

## Features

- Header-only: include `cflag.h`; no compiled library is required.
- Type-safe registration through `var<T>` and `varp<T>`.
- Built-in support for all character, signed integer, unsigned integer, and
  floating-point types, plus `bool`, `std::nullptr_t`, fixed-width integer
  aliases, and `std::string`.
- Conditional support for the C++20 `char8_t` type when compiling as C++20 or
  newer.
- Long flags, short flags, combined Boolean short flags, and positional
  arguments.
- Strict value validation: malformed or partially parsed values are rejected.
- Extensible conversion through `flag_traits<T>`.
- C++11-compatible global state shared safely across translation units.

## Requirements

- A C++11-compatible compiler.
- CMake 3.10 or newer when using the provided CMake target.
- GoogleTest to build the test suite.
- `lcov` and `genhtml` to generate a coverage report.

## Quick start

```cpp
#include "cflag.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    bool daemon = false;
    int port = 0;
    float load_limit = 0.0f;
    std::string config;

    cflag::varp(&daemon, "daemon", "d", false, "run as a daemon.");
    cflag::varp(&port, "port", "p", 8080, "server port.");
    cflag::var(&load_limit, "load-limit", 0.75f, "maximum load.");
    cflag::varp(
            &config,
            "config",
            "c",
            "./config.conf",
            "configuration file.");

    cflag::parse(argc, argv);

    std::cout << "daemon: " << std::boolalpha << daemon << '\n'
              << "port: " << port << '\n'
              << "load limit: " << load_limit << '\n'
              << "config: " << config << '\n';
}
```

Compile it directly; only the include directory is needed:

```shell
c++ -std=c++11 -Iinclude example/example.cc -o example
```

Example invocations:

```shell
./example --port=9000 --daemon
./example --port 9000 --config ./server.conf
./example -dp9000 -c./server.conf
./example --port=9000 -- input.txt --literal-argument
```

`--help` and `-h` are built in:

```text
Usage: ./example [options]

 -c  --config[string] configuration file.(./config.conf)
 -d  --daemon[bool]   run as a daemon.(false)
     --load-limit[float] maximum load.(0.750000)
 -p  --port[int]      server port.(8080)
```

## Flag syntax

| Form | Example | Notes |
| --- | --- | --- |
| Long Boolean | `--daemon` | Sets the value to `true`. |
| Long inline value | `--port=9000` | Supported by every registered type. |
| Long separate value | `--port 9000` | Used by non-Boolean flags. |
| Short Boolean | `-d` | Sets the value to `true`. |
| Combined short Booleans | `-dv` | Equivalent to `-d -v`. |
| Short inline value | `-p9000` | The remainder is the value. |
| Short separate value | `-p 9000` | Consumes the next argument. |
| End of flags | `--` | Every following argument is positional. |

Explicit Boolean values use the long inline form. Accepted true values are
`TRUE`, `True`, `T`, `true`, `t`, and `1`; accepted false values are `FALSE`,
`False`, `F`, `false`, `f`, and `0`.

A single `-` is treated as a positional argument. Parsed positional arguments
are available from `cflag::args()`.

Short names must contain exactly one character. `help` and `h` are reserved
for the built-in help flags. Duplicate names, unknown flags, missing values,
and invalid values are reported to standard error and terminate parsing with
`EXIT_FAILURE`.

## Integration

### Copy the header

Copy [`include/cflag.h`](include/cflag.h) into your include path and include it:

```cpp
#include "cflag.h"
```

### CMake

When the repository is available as a subdirectory:

```cmake
add_subdirectory(path/to/cflag)
target_link_libraries(your_target PRIVATE cflag::cflag)
```

`cflag::cflag` is an `INTERFACE` target. It adds the include directory and the
C++11 compile requirement but does not link a binary library. When `cflag` is
included as a subproject, the example is disabled by default.

Available CMake options:

| Option | Default | Purpose |
| --- | --- | --- |
| `CFLAG_BUILD_EXAMPLE` | On for top-level builds | Build the example program. |
| `CFLAG_BUILD_TESTS` | Off | Build the GoogleTest test suite. |
| `CFLAG_ENABLE_COVERAGE` | Off | Enable GCC/Clang coverage instrumentation. |

## API

### Register flags

```cpp
template <typename T>
void cflag::var(
        T *target,
        const std::string &name,
        const T &default_value,
        const std::string &usage);

template <typename T>
void cflag::varp(
        T *target,
        const std::string &name,
        const std::string &short_name,
        const T &default_value,
        const std::string &usage);
```

`var` registers a long flag. `varp` also assigns a one-character short name.
The target receives the default value during registration and the parsed value
during `parse`.

The target pointer must remain valid until the flag set is reset or is no
longer used.

### Built-in types

| C++ type | Help label | Accepted value |
| --- | --- | --- |
| `bool` | `bool` | Documented Boolean spellings |
| `char`, `signed char`, `unsigned char` | Corresponding type name | Numeric code-unit value within the target type's range |
| `wchar_t`, `char16_t`, `char32_t` | Corresponding type name | Numeric code-unit value within the target type's range |
| `char8_t` (C++20) | `char8_t` | Numeric code-unit value within the target type's range |
| `short`, `int`, `long`, `long long` | Corresponding type name | Decimal value within the target type's signed range |
| Their unsigned variants | Corresponding type name | Decimal value from `0` through the target type's maximum |
| Fixed-width integer aliases from `<cstdint>` | Underlying or fixed-width type name | Decimal value within the exact target range |
| `float`, `double`, `long double` | Corresponding type name | A value fully accepted by `std::stof`, `std::stod`, or `std::stold` |
| `std::nullptr_t` | `nullptr` | The exact text `nullptr` |
| `std::string` | `string` | Any string, including an explicit empty value |

Character flags use numeric code-unit values; they do not decode a textual
character or UTF encoding. For example, `--letter=65` stores the value
represented by code unit 65 in a `char`.

Include `<cstdint>` when declaring fixed-width variables. Fixed-width types are
aliases of fundamental integer types, so C++ cannot always preserve the
spelling used at registration. The help label may therefore show either the
underlying type or a fixed-width name. Parsing always uses the target type's
`std::numeric_limits` bounds.

`void` is the only C++11 fundamental type that cannot be registered: C++ does
not permit an object or flag target of type `void`.

### Parse and inspect arguments

```cpp
void cflag::parse(int argc, char *argv[]);
void cflag::parse(const std::vector<std::string> &arguments);
std::vector<std::string> &cflag::args();
void cflag::usage();
void cflag::reset();
```

`parse` updates registered targets and replaces the stored positional
arguments. `reset` removes every registration, positional argument, and stored
program name. Register flags again after calling it.

Registration and parsing mutate the flag set; perform them before sharing
results with worker threads.

### Custom types

Specialize `cflag::flag_traits<T>` to define a display name, default-value
formatting, parsing, and whether a flag supports an implicit value:

```cpp
enum class mode {
    safe,
    fast,
};

namespace cflag {

template <>
struct flag_traits<mode> {
    static const std::string &type_name() {
        static const std::string name = "mode";
        return name;
    }

    static std::string format(mode value) {
        return value == mode::safe ? "safe" : "fast";
    }

    static bool parse(const std::string &text, mode &value) {
        if (text == "safe") {
            value = mode::safe;
            return true;
        }
        if (text == "fast") {
            value = mode::fast;
            return true;
        }
        return false;
    }

    static bool has_implicit_value() {
        return false;
    }
};

} // namespace cflag
```

The type can then be registered normally:

```cpp
mode execution_mode = mode::safe;
cflag::var(
        &execution_mode,
        "mode",
        mode::safe,
        "execution mode.");
```

`parse` should only update its output parameter when conversion succeeds.
Return `false` to reject a value.

## Build and test

The helper script configures and builds the appropriate CMake targets:

```shell
./build.sh             # Release example
./build.sh --debug     # Debug example
./build.sh --test      # Build and run tests
./build.sh --cov       # Tests and HTML coverage report in cov/
./build.sh --clean     # Remove build/ and cov/
```

Without the helper script:

```shell
cmake -S . -B build -DCFLAG_BUILD_EXAMPLE=ON
cmake --build build
./build/bin/example --help
```

## License

Licensed under the [Apache License 2.0](LICENSE).
