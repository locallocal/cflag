# cflag API reference

All symbols live in namespace `cflag`. Global functions operate on one shared
`flag_set`; the same member functions exist on `cflag::flag_set` for
instance-based use.

## Registration

```cpp
template <typename T>
void var(T* target, const std::string& name, const T& default_value,
         const std::string& usage);

template <typename T>
void varp(T* target, const std::string& name, const std::string& short_name,
          const T& default_value, const std::string& usage);
```

- `name` is the long name used as `--name`. `short_name` must be exactly one
  character and is used as `-x`.
- `default_value` is deduced from `T` (via `type_identity`), so a literal that
  does not match `T` fails to compile; cast or construct explicitly.
- `usage` is shown by `--help`; it is word-wrapped so lines stay under 100
  columns.

## Parsing

```cpp
void parse(int argc, char* argv[]);
void parse(const std::vector<std::string>& arguments);  // arguments[0] = program name
void parse_file(const std::string& path,
                flag_file_format format = flag_file_format::automatic);
enum class flag_file_format { automatic, json, yaml, gflags };
```

`parse` sets every target it sees and replaces stored positional arguments.
`parse_file` applies a file only; program name and positional arguments are
untouched.

## Inspection and lifecycle

```cpp
std::vector<std::string>& args();  // positional arguments from the last parse
void usage();                      // print "Usage: <program> [options]" and the flag table
void reset();                      // drop every registration, positional argument and program name
flag_set& global_flag_set();       // the instance behind the global functions
```

`flag_set` also exposes `program()`, `print_flags()` and `args()`.

## Failure behaviour

Every error path prints a message to `std::cerr` and calls
`std::exit(EXIT_FAILURE)`. Messages:

| Situation | Message |
| --- | --- |
| Unknown flag | `flag <name> not exist.` |
| Missing value | `please set flag <name> value.` |
| Rejected value | `invalid value for <name>.` |
| Duplicate registration | `redefine flag <name>.` |
| Reserved name | `flag name is reserved for help.` / `... for flag-file.` |
| Error inside a flag file | `flag file <path>: ...` or `flag file <path>:<line>: ...` |

## Accepted value spellings

- Boolean true: `TRUE`, `True`, `T`, `true`, `t`, `1`; false: `FALSE`,
  `False`, `F`, `false`, `f`, `0`.
- Integers: decimal, must fit the target type's range.
- Floating point: anything `std::stof`/`stod`/`stold` consumes completely.
- Character types take a numeric code unit (`--letter=65`), not a character.
- `std::string`: any text, including empty (`--name=`).
- `std::nullptr_t`: exactly `nullptr`.

## Custom type template

```cpp
enum class mode { safe, fast };

namespace cflag {
template <>
struct flag_traits<mode> {
    static const std::string& type_name() {
        static const std::string name = "mode";
        return name;
    }
    static std::string format(mode value) { return value == mode::safe ? "safe" : "fast"; }
    static bool parse(const std::string& text, mode& value) {
        if (text == "safe") { value = mode::safe; return true; }
        if (text == "fast") { value = mode::fast; return true; }
        return false;  // leave value untouched
    }
    static bool has_implicit_value() { return false; }  // true => "--flag" alone sets "true"
};
}  // namespace cflag

mode execution_mode = mode::safe;
cflag::var(&execution_mode, "mode", mode::safe, "execution mode.");
```

## Threading

Registration and parsing mutate the flag set. Do both before starting threads
that read the targets.
