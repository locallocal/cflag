# cflag
cflag is a C++11 header-only command-line flag library. It provides template-based
registration, built-in support for `bool`, `int`, `float`, and `std::string`, and
an extension point for custom types.

## 1.How to use

```c++
#include "cflag.h"

#include <iostream>

int main(int argc, char *argv[]) {
    int port = 0;
    bool daemon = false;
    bool version = false;
    float point = 0.0;
    std::string ip;
    std::string conf_file;

    cflag::varp(&daemon, "daemon", "d", false, "run with daemonize.");
    cflag::varp(&version, "version", "v", false, "show server version.");
    cflag::varp(&port, "port", "p", 9999, "server tcp port.");
    cflag::varp(&point, "point", "k", 0.0f, "percent of usage.");
    cflag::varp(&conf_file, "config", "c", "./config.conf", "config file of example.");
    cflag::var(&ip, "ip", "0.0.0.0", "server ip address.");
    cflag::parse(argc, argv);

    std::cout << "daemon: " << std::boolalpha << daemon << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "point: " << point << std::endl;
    std::cout << "conf_file: " << conf_file << std::endl;
    return 0;
}
```

Only `include/cflag.h` is required. The example can be compiled directly:

```shell
c++ -std=c++11 -Iinclude example/example.cc -o example
```

Run the CMake-built example:

```shell
# ./build/bin/example --help
Usage: ./build/bin/example [options]

 -c  --config[string]	config file of example.(./config.conf)
 -d  --daemon[bool]	run with daemonize.(false)
     --ip[string]	server ip address.(0.0.0.0)
 -k  --point[float]	percent of usage.(0.000000)
 -p  --port[int]	server tcp port.(9999)
 -v  --version[bool]	show server version.(false)
```

## 2.How to build

The CMake target is an interface library and can be consumed as `cflag::cflag`.

```cmake
add_subdirectory(cflag)
target_link_libraries(your_target PRIVATE cflag::cflag)
```

- build release or debug
```
# ./build.sh
# ./build.sh --debug
```

- build and run tests (GoogleTest is required).
```
# ./build.sh --test
```

- build, run tests, and generate a source coverage report (lcov is required).
```
# ./build.sh --cov
```

- clean directory.
```
# ./build.sh --clean
```

## 3.API

### 3.1 Template registration

- `var<T>(T *target, name, default_value, usage)`
- `varp<T>(T *target, name, short_name, default_value, usage)`

The earlier `bool_var`, `int_var`, `float_var`, and `string_var` APIs remain
available as compatibility wrappers.

### 3.2 Custom types

Custom flag types are supported by specializing `cflag::flag_traits<T>`:

```c++
enum class mode { safe, fast };

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

### 3.3 Parsing

- `parse(int argc, char *argv[])`
- `parse(const std::vector<std::string> &arguments)`
- `reset()`
- `args()`
