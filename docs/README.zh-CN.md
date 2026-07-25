# cflag

[English](../README.md)

`cflag` 是一个基于 C++11 的 header-only 命令行参数库。它通过模板 API
原生支持布尔值、固定宽度整数、浮点数和字符串，也可以通过
`flag_traits<T>` 扩展自定义参数类型。

## 特性

- 仅需包含 `cflag.h`，无需编译或链接二进制库。
- 使用 `var<T>` 和 `varp<T>` 进行类型安全的参数注册。
- 原生支持 `bool`、`int`、`std::uint32_t`、`std::int32_t`、
  `std::uint64_t`、`std::int64_t`、`float` 和 `std::string`。
- 支持长选项、短选项、布尔短选项组合及位置参数。
- 严格校验参数值，拒绝非法值和仅部分转换成功的值。
- 通过 `flag_traits<T>` 扩展自定义类型。
- 兼容 C++11，并保证多个翻译单元共享同一份全局参数状态。

## 环境要求

- 支持 C++11 的编译器。
- 使用项目提供的 CMake target 时，需要 CMake 3.10 或更高版本。
- 构建测试需要 GoogleTest。
- 生成覆盖率报告需要 `lcov` 和 `genhtml`。

## 快速开始

```cpp
#include "cflag.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    bool daemon = false;
    int port = 0;
    float load_limit = 0.0f;
    std::string config;

    cflag::varp(&daemon, "daemon", "d", false, "以守护进程方式运行。");
    cflag::varp(&port, "port", "p", 8080, "服务端口。");
    cflag::var(&load_limit, "load-limit", 0.75f, "最大负载。");
    cflag::varp(
            &config,
            "config",
            "c",
            "./config.conf",
            "配置文件。");

    cflag::parse(argc, argv);

    std::cout << "daemon: " << std::boolalpha << daemon << '\n'
              << "port: " << port << '\n'
              << "load limit: " << load_limit << '\n'
              << "config: " << config << '\n';
}
```

直接编译时只需添加头文件目录：

```shell
c++ -std=c++11 -Iinclude example/example.cc -o example
```

调用示例：

```shell
./example --port=9000 --daemon
./example --port 9000 --config ./server.conf
./example -dp9000 -c./server.conf
./example --port=9000 -- input.txt --literal-argument
```

`--help` 和 `-h` 是内置选项：

```text
Usage: ./example [options]

 -c  --config[string] 配置文件。(./config.conf)
 -d  --daemon[bool]   以守护进程方式运行。(false)
     --load-limit[float] 最大负载。(0.750000)
 -p  --port[int]      服务端口。(8080)
```

## 参数语法

| 形式 | 示例 | 说明 |
| --- | --- | --- |
| 布尔长选项 | `--daemon` | 将值设置为 `true`。 |
| 长选项内联值 | `--port=9000` | 所有已注册类型均支持。 |
| 长选项分离值 | `--port 9000` | 用于非布尔选项。 |
| 布尔短选项 | `-d` | 将值设置为 `true`。 |
| 组合布尔短选项 | `-dv` | 等价于 `-d -v`。 |
| 短选项内联值 | `-p9000` | 剩余字符作为选项值。 |
| 短选项分离值 | `-p 9000` | 使用下一个参数作为选项值。 |
| 结束选项解析 | `--` | 后续参数全部作为位置参数。 |

显式设置布尔值时使用长选项内联形式。真值支持 `TRUE`、`True`、`T`、
`true`、`t` 和 `1`；假值支持 `FALSE`、`False`、`F`、`false`、`f`
和 `0`。

单独的 `-` 会被视为位置参数。解析得到的位置参数可通过
`cflag::args()` 获取。

短选项名称必须是单个字符。`help` 和 `h` 已被内置帮助选项保留。出现
重复名称、未知选项、缺少值或非法值时，解析器会向标准错误输出信息，并以
`EXIT_FAILURE` 终止程序。

## 集成方式

### 直接复制头文件

将 [`include/cflag.h`](../include/cflag.h) 复制到项目的头文件搜索路径，
然后引用：

```cpp
#include "cflag.h"
```

### CMake

将仓库作为子目录使用：

```cmake
add_subdirectory(path/to/cflag)
target_link_libraries(your_target PRIVATE cflag::cflag)
```

`cflag::cflag` 是一个 `INTERFACE` target，只负责传递头文件目录和 C++11
编译要求，不会链接二进制库。作为子项目引入时，默认不会构建示例程序。

可用的 CMake 选项：

| 选项 | 默认值 | 用途 |
| --- | --- | --- |
| `CFLAG_BUILD_EXAMPLE` | 顶层构建时开启 | 构建示例程序。 |
| `CFLAG_BUILD_TESTS` | 关闭 | 构建 GoogleTest 测试。 |
| `CFLAG_ENABLE_COVERAGE` | 关闭 | 启用 GCC/Clang 覆盖率插桩。 |

## API

### 注册参数

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

`var` 注册长选项，`varp` 额外注册一个单字符短选项。注册时会将默认值写入
目标变量，调用 `parse` 后会写入解析得到的值。

目标指针必须保持有效，直到参数集合被重置或不再使用。

### 内置类型

| C++ 类型 | 帮助信息标签 | 可接受的值 |
| --- | --- | --- |
| `bool` | `bool` | 文档列出的布尔值写法 |
| `int` | `int` | 平台 `int` 范围内的十进制整数 |
| `std::int32_t` | `int32` 或 `int` | 精确 32 位有符号范围内的十进制整数 |
| `std::uint32_t` | `uint32` | 从 `0` 到 `UINT32_MAX` 的十进制整数 |
| `std::int64_t` | `int64` 或 `int` | 精确 64 位有符号范围内的十进制整数 |
| `std::uint64_t` | `uint64` | 从 `0` 到 `UINT64_MAX` 的十进制整数 |
| `float` | `float` | 可被 `std::stof` 完整解析的值 |
| `std::string` | `string` | 任意字符串，包括显式传入的空字符串 |

声明固定宽度整数变量时需要包含 `<cstdint>`。如果某个固定宽度有符号类型
是 `int` 的 typedef，C++ 会将两者视为同一类型，因此帮助信息仍显示
`int`；解析范围仍以目标类型的 `std::numeric_limits` 为准。

### 解析和读取位置参数

```cpp
void cflag::parse(int argc, char *argv[]);
void cflag::parse(const std::vector<std::string> &arguments);
std::vector<std::string> &cflag::args();
void cflag::usage();
void cflag::reset();
```

`parse` 会更新已注册的目标变量，并替换上一次保存的位置参数。`reset`
会清除所有注册项、位置参数和程序名称；调用后需要重新注册参数。

注册和解析操作会修改参数集合，建议在启动工作线程前完成这些操作。

### 自定义类型

通过特化 `cflag::flag_traits<T>` 定义类型显示名称、默认值格式化、字符串
解析方式，以及该类型是否支持隐式值：

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

随后可以像内置类型一样注册：

```cpp
mode execution_mode = mode::safe;
cflag::var(
        &execution_mode,
        "mode",
        mode::safe,
        "运行模式。");
```

`parse` 只应在转换成功时修改输出参数；无法转换时返回 `false`。

## 构建和测试

辅助脚本会配置并构建对应的 CMake target：

```shell
./build.sh             # Release 示例
./build.sh --debug     # Debug 示例
./build.sh --test      # 构建并运行测试
./build.sh --cov       # 运行测试并在 cov/ 生成 HTML 覆盖率报告
./build.sh --clean     # 删除 build/ 和 cov/
```

不使用辅助脚本时：

```shell
cmake -S . -B build -DCFLAG_BUILD_EXAMPLE=ON
cmake --build build
./build/bin/example --help
```

## 许可证

本项目使用 [Apache License 2.0](../LICENSE)。
