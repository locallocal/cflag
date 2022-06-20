# cflag
The cflag package contains a C++ library that implements commandline flags processing. It includes built-in support for standard types such as string and the ability to define flags in the source file in which they are used.

## 1.How to use

```c++
#include "cflag.h"

int main(int argc, char *argv[]) {
    int port = 0;
    bool daemon = false;
    bool version = false;
    float point = 0.0;
    std::string ip;
    std::string conf_file;

    cflag::bool_varp(&daemon, "daemon", "d", false, "run with daemonize.");
    cflag::bool_varp(&version, "version", "v", false, "show server version.");
    cflag::int_varp(&port, "port", "p", 9999, "server tcp port.");
    cflag::float_varp(&point, "point", "k", 0.0, "percent of usage.");
    cflag::string_varp(&conf_file, "config", "c", "./config.conf", "config file of example.");
    cflag::string_var(&ip, "ip", "0.0.0.0", "server ip address.");
    cflag::parse(argc, argv);

    std::cout << "daemon: " << std::boolalpha << daemon << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "point: " << point << std::endl;
    std::cout << "conf_file: " << conf_file << std::endl;
    return 0;
}
```

Run example after building.
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
- build release or debug
```
# ./build.sh
# ./build.sh --debug
```

- run tests after build.
```
# ./build.sh --test
```

- run source code coverage analysis after run tests.
```
# ./build.sh --cov
# ll cov
```

- clean directory.
```
# ./build.sh --clean
```

## 3.API
#### 3.1 bool
- `bool_var(bool *arg, const char *name, bool default_value, const char *usage)`

- `bool_var(bool *arg, std::string &name, bool default_value, std::stirng &usage)`

- `bool_varp(bool *arg, const char *name, const char *short_name, bool default_value, const char *usage)`

- `bool_varp(bool *arg, std::string &name, std::string &short_name, bool_default_value, std::string &usage)`

#### 3.2 int
- `int_var(int *arg, const char *name, int default_value, const char *usage)`

- `int_var(int *arg, std::string &name, int default_value, std::string &usage)`

- `int_varp(int *arg, const char *name, const char *short_name, int default_value, const char *usage)`

- `int_varp(int *arg, std::string &name, std::string &short_name, int default_value, std::string &usage)`

#### 3.3 float
- `float_var(float *arg, const char *name, float default_value, const char *usage)`

- `float_var(float *arg, std::string &name, float default_value, std::string &usage)`

- `float_varp(float *arg, const char *name, const char *short_name, float default_value, const char *usage)`

- `float_varp(float *arg, std::string &name, std::string &short_name, float default_value, std::string &usage)`

#### 3.4 string
- `string_var(string *arg, const char *name, const char *default_value, const char *usage)`

- `string_var(string *arg, std::string &name, std::string default_value, std::string &usage)`

- `string_varp(string *arg, const char *name, const char *short_name, const char *default_value, const char *usage)`

- `string_varp(string *arg, std::string &name, std::string &short_name, std::string &default_value, std::string &usage)`

#### 3.5 other
- `parse(int argc, char *argv[])`

- `parse(std::vector<std::string> arguments)`

- `reset()`

- `args()`
