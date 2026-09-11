# Flag files

`--flag-file=<path>` (or `--flag-file <path>`) loads flags from a file.

## Semantics

- Applied at the position where the option appears; later command-line
  arguments override values from the file.
- A file may itself contain `flag-file` to include another file, up to 16
  levels deep. Self-inclusion fails with `is nested too deeply`.
- Relative paths resolve against the current working directory.
- Keys are long flag names. Short names only work in gflags-format lines.
- Errors carry the file path and, for syntax errors, the line number.

## Format detection

1. Extension `.json` → JSON; `.yaml` / `.yml` → YAML.
2. Otherwise the first non-blank, non-comment line decides: `{` → JSON,
   `-` → gflags, anything else → YAML.
3. Force a format from code: `cflag::parse_file(path, cflag::flag_file_format::gflags)`.

## gflags format

One command-line argument per line; blank lines and `#` comments ignored;
positional arguments rejected.

```text
# server.flags
--port=9000
--daemon
-k0.5
--flag-file=common.flags
```

## JSON format

A single flat object. Strings, numbers and Booleans are accepted; `null`,
nested objects and arrays are errors. String escapes including `\uXXXX` and
surrogate pairs are decoded.

```json
{
  "port": 9000,
  "daemon": true,
  "config": "/etc/example.conf",
  "flag-file": "common.json"
}
```

## YAML format (flat subset)

`key: value` lines only. Supported: `#` comments (line or trailing), `---` and
`...` markers, plain scalars, `'single'` (with `''` escape) and `"double"`
(with C-style and `\uXXXX` escapes) scalars, quoted keys, empty values (`key:`
gives an empty string).

Not supported (rejected with a line number): indented/nested mappings, lists
(`- item`), flow collections (`[..]`, `{..}`), block scalars (`|`, `>`),
anchors/aliases/tags (`&`, `*`, `!`).

Plain scalars are literal text: `yes`/`no`/`on`/`off` are **not** Booleans,
and `~`/`null` are not null.

```yaml
---
port: 9000            # trailing comment
daemon: true
config: "/etc/example.conf"
name: 'it''s quoted'
```
