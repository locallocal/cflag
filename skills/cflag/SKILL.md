---
name: cflag
description: Integrate the cflag header-only C++11 command-line flag library into a project and use it correctly. Use when a C++ project needs to add or change command-line flags, wire cflag in via CMake or a copied header, load flags from JSON/YAML/gflags files with --flag-file, or register a custom flag type through flag_traits<T>.
---

# cflag

cflag is a single-header (`include/cflag.h`), dependency-free C++11 flag parser.
Follow the steps below in order; consult `references/` only when the step needs
detail.

## 1. Integrate

Pick one, preferring CMake when the project already uses it:

- **Subdirectory**: vendor or submodule the repository, then
  `add_subdirectory(third_party/cflag)` and
  `target_link_libraries(<target> PRIVATE cflag::cflag)`.
- **FetchContent**: see `templates/CMakeLists.cflag.cmake`. Pin `GIT_TAG` to a
  tag or commit, never a moving branch.
- **Copy the header**: copy `include/cflag.h` into the project's include path.
  Only do this when the project has no build system that can add a dependency.

`cflag::cflag` is an INTERFACE target: it adds the include directory and the
C++11 requirement, nothing to link. Subprojects get the example and tests
disabled automatically.

## 2. Register flags, then parse

Start from `templates/main.cc`. Rules that matter:

- Register every flag with `cflag::var` (long name only) or `cflag::varp`
  (long + one-character short name) **before** calling `cflag::parse`.
- The target variable receives the default at registration and the parsed
  value at `parse`. It must outlive the flag set.
- Pass the default with the target's exact type. `9999` for an `int`,
  `0.5f` for a `float`, `std::string("x")` or a string literal for
  `std::string`.
- Names `help`, `h` and `flag-file` are reserved. Duplicate names, unknown
  flags, missing values and invalid values print to stderr and call
  `std::exit(EXIT_FAILURE)`; `--help`/`-h` print usage and exit 0. Do not
  wrap `parse` in try/catch expecting exceptions.
- Positional arguments come back from `cflag::args()`. `--` ends flag parsing.
- Booleans: `--daemon` / `-d` set true; explicit values use `--daemon=false`.
  Non-Boolean flags accept `--port=9000`, `--port 9000`, `-p9000`, `-p 9000`.
- Prefer the global functions for a normal program. Use a `cflag::flag_set`
  instance only for libraries or tests that must not share global state.

Supported target types: `bool`, every character, integer and floating-point
type, fixed-width `<cstdint>` aliases, `std::nullptr_t`, `std::string`. Values
are validated strictly (no partial parses, range checked).

## 3. Flag files (optional)

`--flag-file=<path>` is built in. It applies the file at that position, later
arguments override it, and files may include other files with `flag-file`
(16 levels max). Format by extension (`.json`, `.yaml`, `.yml`), otherwise
sniffed; force it with `cflag::parse_file(path, cflag::flag_file_format::yaml)`.

Only flat key/value content is accepted in JSON and YAML; nested objects,
arrays/lists, `null`, block scalars, anchors and tags are errors. Details and
examples: `references/flag-files.md`.

## 4. Custom types (optional)

Specialize `cflag::flag_traits<T>` with `type_name()`, `format()`, `parse()`
and `has_implicit_value()`; then register the type like any other. Template in
`references/api.md`. `parse` must leave the output untouched and return false
on bad input.

## 5. Verify

Build, then run `<binary> --help` and confirm every flag appears with its type
and default. Run once with a wrong value (e.g. `--port=abc`) to see the
failure path exit non-zero. If the project has tests, add one that calls
`flag_set::parse` with a `std::vector<std::string>` instead of `argv`.
