// Copyright 2022 locallocal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace cflag {

class flag_set;
class flag;
class value;

// File format accepted by flag_set::parse_file(). automatic picks the format
// from the file extension, falling back to a content sniff.
enum class flag_file_format { automatic, json, yaml, gflags };

namespace detail {

inline const std::string& bool_type_name() {
    static const std::string value = "bool";
    return value;
}

inline const std::string& int_type_name() {
    static const std::string value = "int";
    return value;
}

inline const std::string& int32_type_name() {
    static const std::string value = "int32";
    return value;
}

inline const std::string& uint32_type_name() {
    static const std::string value = "uint32";
    return value;
}

inline const std::string& int64_type_name() {
    static const std::string value = "int64";
    return value;
}

inline const std::string& uint64_type_name() {
    static const std::string value = "uint64";
    return value;
}

inline const std::string& char_type_name() {
    static const std::string value = "char";
    return value;
}

inline const std::string& signed_char_type_name() {
    static const std::string value = "signed char";
    return value;
}

inline const std::string& unsigned_char_type_name() {
    static const std::string value = "unsigned char";
    return value;
}

inline const std::string& wchar_type_name() {
    static const std::string value = "wchar_t";
    return value;
}

inline const std::string& char16_type_name() {
    static const std::string value = "char16_t";
    return value;
}

inline const std::string& char32_type_name() {
    static const std::string value = "char32_t";
    return value;
}

#if defined(__cpp_char8_t)
inline const std::string& char8_type_name() {
    static const std::string value = "char8_t";
    return value;
}
#endif

inline const std::string& short_type_name() {
    static const std::string value = "short";
    return value;
}

inline const std::string& unsigned_short_type_name() {
    static const std::string value = "unsigned short";
    return value;
}

inline const std::string& unsigned_int_type_name() {
    static const std::string value = "unsigned int";
    return value;
}

inline const std::string& long_type_name() {
    static const std::string value = "long";
    return value;
}

inline const std::string& unsigned_long_type_name() {
    static const std::string value = "unsigned long";
    return value;
}

inline const std::string& long_long_type_name() {
    static const std::string value = "long long";
    return value;
}

inline const std::string& unsigned_long_long_type_name() {
    static const std::string value = "unsigned long long";
    return value;
}

inline const std::string& float_type_name() {
    static const std::string value = "float";
    return value;
}

inline const std::string& double_type_name() {
    static const std::string value = "double";
    return value;
}

inline const std::string& long_double_type_name() {
    static const std::string value = "long double";
    return value;
}

inline const std::string& nullptr_type_name() {
    static const std::string value = "nullptr";
    return value;
}

inline const std::string& string_type_name() {
    static const std::string value = "string";
    return value;
}

inline const std::string& help_flag_name() {
    static const std::string value = "help";
    return value;
}

inline const std::string& help_short_flag_name() {
    static const std::string value = "h";
    return value;
}

// Maximum width of a line printed by flag_set::print_flags().
inline std::size_t usage_line_width() { return 100; }

// Minimum width reserved for the usage column, even when the labels are very wide.
inline std::size_t usage_min_text_width() { return 20; }

// Splits text into lines no longer than width, breaking on spaces where possible.
inline std::vector<std::string> wrap_text(const std::string& text, std::size_t width) {
    std::vector<std::string> lines;
    std::string line;
    std::size_t position = 0;

    while (position < text.size()) {
        std::size_t word_end = text.find(' ', position);
        if (word_end == std::string::npos) {
            word_end = text.size();
        }
        std::string word = text.substr(position, word_end - position);
        position = word_end + 1;
        if (word.empty()) {
            continue;
        }

        while (word.size() > width) {
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
            }
            lines.push_back(word.substr(0, width));
            word.erase(0, width);
        }

        if (line.empty()) {
            line = word;
        } else if (line.size() + 1 + word.size() <= width) {
            line += ' ';
            line += word;
        } else {
            lines.push_back(line);
            line = word;
        }
    }

    if (!line.empty() || lines.empty()) {
        lines.push_back(line);
    }
    return lines;
}

[[noreturn]] inline void fail(const std::string& message) {
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

inline const std::string& flag_file_flag_name() {
    static const std::string value = "flag-file";
    return value;
}

// Maximum nesting depth of --flag-file references, guarding against cycles.
inline std::size_t max_flag_file_depth() { return 16; }

inline bool is_blank(char character) {
    return character == ' ' || character == '\t' || character == '\r' || character == '\n';
}

inline std::string trim(const std::string& text) {
    std::size_t begin = 0;
    while (begin < text.size() && is_blank(text[begin])) {
        ++begin;
    }
    std::size_t end = text.size();
    while (end > begin && is_blank(text[end - 1])) {
        --end;
    }
    return text.substr(begin, end - begin);
}

inline std::vector<std::string> split_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::string line;
    for (std::size_t index = 0; index < text.size(); ++index) {
        if (text[index] == '\n') {
            lines.push_back(line);
            line.clear();
        } else {
            line += text[index];
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }
    for (std::size_t index = 0; index < lines.size(); ++index) {
        if (!lines[index].empty() && lines[index][lines[index].size() - 1] == '\r') {
            lines[index].erase(lines[index].size() - 1);
        }
    }
    return lines;
}

inline bool read_file_text(const std::string& path, std::string& text) {
    std::ifstream input(path.c_str(), std::ios::in | std::ios::binary);
    if (!input) {
        return false;
    }
    text.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    return true;
}

inline void append_utf8(unsigned long code_point, std::string& output) {
    if (code_point < 0x80) {
        output += static_cast<char>(code_point);
    } else if (code_point < 0x800) {
        output += static_cast<char>(0xC0 | (code_point >> 6));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    } else if (code_point < 0x10000) {
        output += static_cast<char>(0xE0 | (code_point >> 12));
        output += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    } else {
        output += static_cast<char>(0xF0 | (code_point >> 18));
        output += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
        output += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    }
}

inline bool parse_hex4(const std::string& text, std::size_t position, unsigned long& code_unit) {
    if (position + 4 > text.size()) {
        return false;
    }
    code_unit = 0;
    for (std::size_t index = 0; index < 4; ++index) {
        const char digit = text[position + index];
        code_unit <<= 4;
        if (digit >= '0' && digit <= '9') {
            code_unit |= static_cast<unsigned long>(digit - '0');
        } else if (digit >= 'a' && digit <= 'f') {
            code_unit |= static_cast<unsigned long>(digit - 'a' + 10);
        } else if (digit >= 'A' && digit <= 'F') {
            code_unit |= static_cast<unsigned long>(digit - 'A' + 10);
        } else {
            return false;
        }
    }
    return true;
}

[[noreturn]] inline void fail_in_file(const std::string& path, std::size_t line, const std::string& message) {
    fail("flag file " + path + ":" + std::to_string(line) + ": " + message);
}

// Reads a gflags style file: one --name=value (or --name / -x...) argument per line.
// Blank lines and lines starting with '#' are ignored.
inline std::vector<std::string> read_gflags_flag_file(const std::string& path, const std::string& text) {
    const std::vector<std::string> lines = split_lines(text);
    std::vector<std::string> arguments;
    for (std::size_t index = 0; index < lines.size(); ++index) {
        const std::string line = trim(lines[index]);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line[0] != '-') {
            fail_in_file(path, index + 1, "expected a flag starting with '-'.");
        }
        arguments.push_back(line);
    }
    return arguments;
}

// Reads a JSON object whose members are flag names mapped to scalar values.
class json_flag_file_reader {
public:
    json_flag_file_reader(const std::string& path, const std::string& text) : path_(path), text_(text), position_(0) {}

    std::vector<std::string> read() {
        std::vector<std::string> arguments;
        skip_whitespace_();
        expect_('{', "expected '{' at the start of the flag file.");
        skip_whitespace_();
        if (!consume_('}')) {
            while (true) {
                skip_whitespace_();
                if (peek_() != '"') {
                    fail_("expected a quoted flag name.");
                }
                const std::string name = parse_string_();
                if (name.empty()) {
                    fail_("flag name cannot be empty.");
                }
                skip_whitespace_();
                expect_(':', "expected ':' after the flag name.");
                skip_whitespace_();
                const std::string value = parse_scalar_();
                arguments.push_back("--" + name + "=" + value);
                skip_whitespace_();
                if (consume_(',')) {
                    continue;
                }
                expect_('}', "expected ',' or '}' after the value.");
                break;
            }
        }
        skip_whitespace_();
        if (position_ != text_.size()) {
            fail_("unexpected content after the closing '}'.");
        }
        return arguments;
    }

private:
    [[noreturn]] void fail_(const std::string& message) const {
        std::size_t line = 1;
        for (std::size_t index = 0; index < position_ && index < text_.size(); ++index) {
            if (text_[index] == '\n') {
                ++line;
            }
        }
        fail_in_file(path_, line, message);
    }

    char peek_() const { return position_ < text_.size() ? text_[position_] : '\0'; }

    void skip_whitespace_() {
        while (position_ < text_.size() && is_blank(text_[position_])) {
            ++position_;
        }
    }

    bool consume_(char character) {
        if (peek_() != character) {
            return false;
        }
        ++position_;
        return true;
    }

    void expect_(char character, const std::string& message) {
        if (!consume_(character)) {
            fail_(message);
        }
    }

    bool consume_word_(const char* word) {
        const std::size_t length = std::string(word).size();
        if (text_.compare(position_, length, word) != 0) {
            return false;
        }
        position_ += length;
        return true;
    }

    std::string parse_scalar_() {
        const char character = peek_();
        if (character == '"') {
            return parse_string_();
        }
        if (character == '{' || character == '[') {
            fail_("nested objects and arrays are not supported.");
        }
        if (consume_word_("true")) {
            return "true";
        }
        if (consume_word_("false")) {
            return "false";
        }
        if (consume_word_("null")) {
            fail_("null values are not supported.");
        }
        const std::size_t begin = position_;
        while (position_ < text_.size()) {
            const char digit = text_[position_];
            const bool numeric = (digit >= '0' && digit <= '9') || digit == '-' || digit == '+' || digit == '.' ||
                                 digit == 'e' || digit == 'E';
            if (!numeric) {
                break;
            }
            ++position_;
        }
        if (begin == position_) {
            fail_("expected a value.");
        }
        return text_.substr(begin, position_ - begin);
    }

    std::string parse_string_() {
        expect_('"', "expected '\"'.");
        std::string result;
        while (true) {
            if (position_ >= text_.size()) {
                fail_("unterminated string.");
            }
            const char character = text_[position_++];
            if (character == '"') {
                return result;
            }
            if (character != '\\') {
                result += character;
                continue;
            }
            if (position_ >= text_.size()) {
                fail_("unterminated string.");
            }
            const char escape = text_[position_++];
            switch (escape) {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '/':
                    result += '/';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'u':
                    parse_unicode_escape_(result);
                    break;
                default:
                    fail_("invalid escape sequence.");
            }
        }
    }

    void parse_unicode_escape_(std::string& result) {
        unsigned long code_point = 0;
        if (!parse_hex4(text_, position_, code_point)) {
            fail_("invalid \\u escape sequence.");
        }
        position_ += 4;
        if (code_point >= 0xD800 && code_point <= 0xDBFF) {
            unsigned long low = 0;
            if (text_.compare(position_, 2, "\\u") != 0 || !parse_hex4(text_, position_ + 2, low) || low < 0xDC00 ||
                low > 0xDFFF) {
                fail_("invalid surrogate pair in \\u escape sequence.");
            }
            position_ += 6;
            code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low - 0xDC00);
        }
        append_utf8(code_point, result);
    }

    std::string path_;
    std::string text_;
    std::size_t position_;
};

// Reads a flat YAML mapping of flag names to scalar values. Nested mappings,
// lists, block scalars, anchors and tags are rejected.
class yaml_flag_file_reader {
public:
    yaml_flag_file_reader(const std::string& path, const std::string& text) : path_(path), text_(text) {}

    std::vector<std::string> read() const {
        const std::vector<std::string> lines = split_lines(text_);
        std::vector<std::string> arguments;
        for (std::size_t index = 0; index < lines.size(); ++index) {
            const std::size_t line_number = index + 1;
            const std::string& raw = lines[index];
            const std::string trimmed = trim(raw);
            if (trimmed.empty() || trimmed[0] == '#' || trimmed == "---" || trimmed == "...") {
                continue;
            }
            if (is_blank(raw[0])) {
                fail_in_file(path_, line_number, "nested mappings are not supported.");
            }
            if (trimmed == "-" || trimmed.compare(0, 2, "- ") == 0) {
                fail_in_file(path_, line_number, "lists are not supported.");
            }

            std::size_t position = 0;
            const std::string name = parse_key_(raw, position, line_number);
            if (name.empty()) {
                fail_in_file(path_, line_number, "flag name cannot be empty.");
            }
            const std::string value = parse_value_(raw, position, line_number);
            arguments.push_back("--" + name + "=" + value);
        }
        return arguments;
    }

private:
    std::string parse_key_(const std::string& line, std::size_t& position, std::size_t line_number) const {
        std::string name;
        if (line[0] == '"' || line[0] == '\'') {
            name = parse_quoted_(line, position, line_number);
            while (position < line.size() && is_blank(line[position])) {
                ++position;
            }
            if (position >= line.size() || line[position] != ':') {
                fail_in_file(path_, line_number, "expected ':' after the flag name.");
            }
            ++position;
            return name;
        }
        for (std::size_t index = 0; index < line.size(); ++index) {
            if (line[index] == ':' && (index + 1 == line.size() || is_blank(line[index + 1]))) {
                position = index + 1;
                return trim(line.substr(0, index));
            }
        }
        fail_in_file(path_, line_number, "expected 'name: value'.");
    }

    std::string parse_value_(const std::string& line, std::size_t& position, std::size_t line_number) const {
        while (position < line.size() && is_blank(line[position])) {
            ++position;
        }
        if (position >= line.size() || line[position] == '#') {
            return std::string();
        }
        if (line[position] == '"' || line[position] == '\'') {
            const std::string value = parse_quoted_(line, position, line_number);
            const std::string rest = trim(line.substr(position));
            if (!rest.empty() && rest[0] != '#') {
                fail_in_file(path_, line_number, "unexpected content after the quoted value.");
            }
            return value;
        }

        std::string plain = line.substr(position);
        for (std::size_t index = 1; index < plain.size(); ++index) {
            if (plain[index] == '#' && is_blank(plain[index - 1])) {
                plain.erase(index);
                break;
            }
        }
        plain = trim(plain);
        const char first = plain[0];
        if (first == '[' || first == '{') {
            fail_in_file(path_, line_number, "flow collections are not supported.");
        }
        if (first == '|' || first == '>') {
            fail_in_file(path_, line_number, "block scalars are not supported.");
        }
        if (first == '&' || first == '*' || first == '!') {
            fail_in_file(path_, line_number, "anchors, aliases and tags are not supported.");
        }
        return plain;
    }

    std::string parse_quoted_(const std::string& line, std::size_t& position, std::size_t line_number) const {
        const char quote = line[position++];
        std::string result;
        while (true) {
            if (position >= line.size()) {
                fail_in_file(path_, line_number, "unterminated quoted string.");
            }
            const char character = line[position++];
            if (quote == '\'') {
                if (character != '\'') {
                    result += character;
                } else if (position < line.size() && line[position] == '\'') {
                    result += '\'';
                    ++position;
                } else {
                    return result;
                }
                continue;
            }
            if (character == '"') {
                return result;
            }
            if (character != '\\') {
                result += character;
                continue;
            }
            if (position >= line.size()) {
                fail_in_file(path_, line_number, "unterminated quoted string.");
            }
            const char escape = line[position++];
            switch (escape) {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '/':
                    result += '/';
                    break;
                case '0':
                    result += '\0';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'u': {
                    unsigned long code_point = 0;
                    if (!parse_hex4(line, position, code_point)) {
                        fail_in_file(path_, line_number, "invalid \\u escape sequence.");
                    }
                    position += 4;
                    append_utf8(code_point, result);
                    break;
                }
                default:
                    fail_in_file(path_, line_number, "invalid escape sequence.");
            }
        }
    }

    std::string path_;
    std::string text_;
};

inline flag_file_format detect_flag_file_format(const std::string& path, const std::string& text) {
    const std::size_t dot = path.find_last_of('.');
    const std::size_t slash = path.find_last_of("/\\");
    if (dot != std::string::npos && (slash == std::string::npos || dot > slash)) {
        std::string extension = path.substr(dot + 1);
        for (std::size_t index = 0; index < extension.size(); ++index) {
            if (extension[index] >= 'A' && extension[index] <= 'Z') {
                extension[index] = static_cast<char>(extension[index] - 'A' + 'a');
            }
        }
        if (extension == "json") {
            return flag_file_format::json;
        }
        if (extension == "yaml" || extension == "yml") {
            return flag_file_format::yaml;
        }
    }

    const std::vector<std::string> lines = split_lines(text);
    for (std::size_t index = 0; index < lines.size(); ++index) {
        const std::string line = trim(lines[index]);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line[0] == '{') {
            return flag_file_format::json;
        }
        if (line[0] == '-' && (line.size() == 1 || !is_blank(line[1]))) {
            return flag_file_format::gflags;
        }
        return flag_file_format::yaml;
    }
    return flag_file_format::gflags;
}

template <typename>
struct dependent_false : std::false_type {};

template <typename T>
struct type_identity {
    typedef T type;
};

template <typename T>
struct is_supported_integer
    : std::integral_constant<bool, std::is_integral<T>::value && !std::is_same<T, bool>::value> {};

template <typename T>
struct floating_parser;

template <>
struct floating_parser<float> {
    static float parse(const std::string& value, std::size_t* parsed_length) { return std::stof(value, parsed_length); }
};

template <>
struct floating_parser<double> {
    static double parse(const std::string& value, std::size_t* parsed_length) {
        return std::stod(value, parsed_length);
    }
};

template <>
struct floating_parser<long double> {
    static long double parse(const std::string& value, std::size_t* parsed_length) {
        return std::stold(value, parsed_length);
    }
};

}  // namespace detail

// Specialize flag_traits for a custom type to use it with var<T>/varp<T>.
template <typename T, typename Enable = void>
struct flag_traits {
    static_assert(detail::dependent_false<T>::value, "cflag::flag_traits<T> must be specialized for this flag type");
};

template <>
struct flag_traits<bool> {
    static const std::string& type_name() { return detail::bool_type_name(); }

    static std::string format(bool value) { return value ? "true" : "false"; }

    static bool parse(const std::string& value, bool& output) {
        if (value == "TRUE" || value == "True" || value == "T" || value == "true" || value == "t" || value == "1") {
            output = true;
            return true;
        }
        if (value == "FALSE" || value == "False" || value == "F" || value == "false" || value == "f" || value == "0") {
            output = false;
            return true;
        }
        return false;
    }

    static bool has_implicit_value() { return true; }
};

template <typename T>
struct flag_traits<T, typename std::enable_if<detail::is_supported_integer<T>::value>::type> {
    static const std::string& type_name() {
        if (std::is_same<T, int>::value) {
            return detail::int_type_name();
        }
        if (std::is_same<T, std::int32_t>::value) {
            return detail::int32_type_name();
        }
        if (std::is_same<T, std::uint32_t>::value) {
            return detail::uint32_type_name();
        }
        if (std::is_same<T, std::int64_t>::value) {
            return detail::int64_type_name();
        }
        if (std::is_same<T, std::uint64_t>::value) {
            return detail::uint64_type_name();
        }
        if (std::is_same<T, char>::value) {
            return detail::char_type_name();
        }
        if (std::is_same<T, signed char>::value) {
            return detail::signed_char_type_name();
        }
        if (std::is_same<T, unsigned char>::value) {
            return detail::unsigned_char_type_name();
        }
        if (std::is_same<T, wchar_t>::value) {
            return detail::wchar_type_name();
        }
        if (std::is_same<T, char16_t>::value) {
            return detail::char16_type_name();
        }
        if (std::is_same<T, char32_t>::value) {
            return detail::char32_type_name();
        }
#if defined(__cpp_char8_t)
        if (std::is_same<T, char8_t>::value) {
            return detail::char8_type_name();
        }
#endif
        if (std::is_same<T, short>::value) {
            return detail::short_type_name();
        }
        if (std::is_same<T, unsigned short>::value) {
            return detail::unsigned_short_type_name();
        }
        if (std::is_same<T, unsigned int>::value) {
            return detail::unsigned_int_type_name();
        }
        if (std::is_same<T, long>::value) {
            return detail::long_type_name();
        }
        if (std::is_same<T, unsigned long>::value) {
            return detail::unsigned_long_type_name();
        }
        if (std::is_same<T, long long>::value) {
            return detail::long_long_type_name();
        }
        return detail::unsigned_long_long_type_name();
    }

    static std::string format(T value) { return std::to_string(value); }

    static bool parse(const std::string& value, T& output) {
        return parse_value(value, output, std::integral_constant<bool, std::numeric_limits<T>::is_signed>());
    }

    static bool has_implicit_value() { return false; }

private:
    static bool parse_value(const std::string& value, T& output, std::true_type) {
        try {
            std::size_t parsed_length = 0;
            const long long parsed_value = std::stoll(value, &parsed_length);
            if (parsed_length != value.size()) {
                return false;
            }
            if (parsed_value < static_cast<long long>(std::numeric_limits<T>::min()) ||
                parsed_value > static_cast<long long>(std::numeric_limits<T>::max())) {
                return false;
            }
            output = static_cast<T>(parsed_value);
            return true;
        } catch (const std::invalid_argument&) {
            return false;
        } catch (const std::out_of_range&) {
            return false;
        }
    }

    static bool parse_value(const std::string& value, T& output, std::false_type) {
        const std::size_t first_character = value.find_first_not_of(" \t\n\r\f\v");
        if (first_character == std::string::npos || value[first_character] == '-') {
            return false;
        }

        try {
            std::size_t parsed_length = 0;
            const unsigned long long parsed_value = std::stoull(value, &parsed_length);
            if (parsed_length != value.size()) {
                return false;
            }
            if (parsed_value > static_cast<unsigned long long>(std::numeric_limits<T>::max())) {
                return false;
            }
            output = static_cast<T>(parsed_value);
            return true;
        } catch (const std::invalid_argument&) {
            return false;
        } catch (const std::out_of_range&) {
            return false;
        }
    }
};

template <typename T>
struct flag_traits<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    static const std::string& type_name() {
        if (std::is_same<T, float>::value) {
            return detail::float_type_name();
        }
        if (std::is_same<T, double>::value) {
            return detail::double_type_name();
        }
        return detail::long_double_type_name();
    }

    static std::string format(T value) { return std::to_string(value); }

    static bool parse(const std::string& value, T& output) {
        try {
            std::size_t parsed_length = 0;
            const T parsed_value = detail::floating_parser<T>::parse(value, &parsed_length);
            if (parsed_length != value.size()) {
                return false;
            }
            output = parsed_value;
            return true;
        } catch (const std::invalid_argument&) {
            return false;
        } catch (const std::out_of_range&) {
            return false;
        }
    }

    static bool has_implicit_value() { return false; }
};

template <>
struct flag_traits<std::nullptr_t> {
    static const std::string& type_name() { return detail::nullptr_type_name(); }

    static std::string format(std::nullptr_t) { return "nullptr"; }

    static bool parse(const std::string& value, std::nullptr_t& output) {
        if (value != "nullptr") {
            return false;
        }
        output = nullptr;
        return true;
    }

    static bool has_implicit_value() { return false; }
};

template <>
struct flag_traits<std::string> {
    static const std::string& type_name() { return detail::string_type_name(); }

    static const std::string& format(const std::string& value) { return value; }

    static bool parse(const std::string& value, std::string& output) {
        output = value;
        return true;
    }

    static bool has_implicit_value() { return false; }
};

class value {
public:
    virtual ~value() = default;
    virtual bool set(const std::string& text) = 0;
    virtual const std::string& type() const = 0;
    virtual bool has_implicit_value() const = 0;
};

template <typename T>
class c_value final : public value {
public:
    explicit c_value(T* arg) : arg_(arg) {}

    bool set(const std::string& text) override {
        if (arg_ == nullptr) {
            return false;
        }
        T parsed_value = *arg_;
        if (!flag_traits<T>::parse(text, parsed_value)) {
            return false;
        }
        *arg_ = parsed_value;
        return true;
    }

    const std::string& type() const override { return flag_traits<T>::type_name(); }

    bool has_implicit_value() const override { return flag_traits<T>::has_implicit_value(); }

private:
    T* arg_;
};

class flag {
public:
    explicit flag(const std::string& name) : name_(name) {}

    void short_name(const std::string& short_name) { short_name_ = short_name; }

    void usage(const std::string& usage) { usage_ = usage; }

    void default_value(const std::string& default_value) { default_value_ = default_value; }

    void value(const std::shared_ptr<cflag::value>& value) { value_ = value; }

    const std::string& name() const { return name_; }

    const std::string& usage() const { return usage_; }

    const std::string& short_name() const { return short_name_; }

    const std::string& default_value() const { return default_value_; }

    const std::shared_ptr<cflag::value>& value() const { return value_; }

private:
    std::string name_;
    std::string short_name_;
    std::string usage_;
    std::string default_value_;
    std::shared_ptr<cflag::value> value_;
};

class flag_set {
public:
    flag_set() = default;
    flag_set(const flag_set&) = delete;
    flag_set(flag_set&&) = delete;
    flag_set& operator=(const flag_set&) = delete;
    flag_set& operator=(flag_set&&) = delete;

    void usage() const;
    void print_flags() const;
    void parse(int argc, char* argv[]);
    void parse(const std::vector<std::string>& arguments);
    void parse_file(const std::string& path, flag_file_format format = flag_file_format::automatic);
    void reset();

    template <typename T>
    void var(T* arg, const std::string& name, const typename detail::type_identity<T>::type& default_value,
             const std::string& usage);

    template <typename T>
    void varp(T* arg, const std::string& name, const std::string& short_name,
              const typename detail::type_identity<T>::type& default_value, const std::string& usage);

    void program(const std::string& value) { program_ = value; }

    const std::string& program() const { return program_; }

    std::vector<std::string>& args() { return args_; }

    const std::vector<std::string>& args() const { return args_; }

private:
    std::shared_ptr<flag> lookup_(const std::string& name, bool short_name) const;
    void add_flag_(const std::shared_ptr<flag>& flag);
    void parse_arguments_(const std::vector<std::string>& arguments, std::size_t begin, bool allow_positional);
    void parse_long_args_(const std::string& segment, std::size_t& index, const std::vector<std::string>& arguments);
    void parse_short_args_(const std::string& segment, std::size_t& index, const std::vector<std::string>& arguments);
    [[noreturn]] void fail_(const std::string& message) const;

    std::string program_;
    std::map<std::string, std::shared_ptr<flag>> flags_;
    std::map<std::string, std::shared_ptr<flag>> short_flags_;
    std::vector<std::string> args_;
    std::size_t flag_file_depth_ = 0;
    std::string flag_file_context_;
};

template <typename T>
inline void flag_set::var(T* arg, const std::string& name, const typename detail::type_identity<T>::type& default_value,
                          const std::string& usage_text) {
    varp(arg, name, "", default_value, usage_text);
}

template <typename T>
inline void flag_set::varp(T* arg, const std::string& name, const std::string& short_name,
                           const typename detail::type_identity<T>::type& default_value,
                           const std::string& usage_text) {
    if (arg == nullptr) {
        detail::fail("flag target cannot be null.");
    }

    std::shared_ptr<flag> new_flag = std::make_shared<flag>(name);
    new_flag->short_name(short_name);
    new_flag->usage(usage_text);
    new_flag->default_value(flag_traits<T>::format(default_value));
    new_flag->value(std::make_shared<c_value<T>>(arg));

    add_flag_(new_flag);
    *arg = default_value;
}

inline void flag_set::usage() const {
    std::cout << "Usage: " << program() << " [options]\n\n";
    print_flags();
}

inline void flag_set::print_flags() const {
    std::vector<std::string> flag_labels;
    flag_labels.reserve(flags_.size());
    std::size_t label_width = 0;

    for (const auto& entry : flags_) {
        const std::shared_ptr<flag>& flag = entry.second;
        std::string label;
        if (!flag->short_name().empty()) {
            label = " -" + flag->short_name() + "  ";
        } else {
            label = "     ";
        }
        if (!flag->name().empty()) {
            label += "--" + flag->name();
        } else {
            label += "    ";
        }
        label += '[';
        label += flag->value()->type();
        label += ']';
        if (label.size() > label_width) {
            label_width = label.size();
        }
        flag_labels.push_back(label);
    }

    const std::size_t text_column = label_width + 1;
    const std::size_t text_width = text_column + detail::usage_min_text_width() <= detail::usage_line_width()
                                       ? detail::usage_line_width() - text_column
                                       : detail::usage_min_text_width();

    std::size_t index = 0;
    for (const auto& entry : flags_) {
        const std::shared_ptr<flag>& flag = entry.second;
        const std::string& label = flag_labels[index++];
        std::string text = flag->usage();
        if (!flag->default_value().empty()) {
            text += '(';
            text += flag->default_value();
            text += ')';
        }

        const std::vector<std::string> lines = detail::wrap_text(text, text_width);
        std::cout << label << std::string(text_column - label.size(), ' ') << lines[0] << '\n';
        for (std::size_t line_index = 1; line_index < lines.size(); ++line_index) {
            std::cout << std::string(text_column, ' ') << lines[line_index] << '\n';
        }
    }
}

inline void flag_set::parse(int argc, char* argv[]) {
    if (argc > 0 && argv == nullptr) {
        detail::fail("argument vector cannot be null.");
    }

    std::vector<std::string> arguments;
    arguments.reserve(argc > 0 ? static_cast<std::size_t>(argc) : 0);
    for (int index = 0; index < argc; ++index) {
        arguments.push_back(argv[index]);
    }
    parse(arguments);
}

inline void flag_set::parse(const std::vector<std::string>& arguments) {
    args_.clear();
    if (arguments.empty()) {
        program_.clear();
        return;
    }

    program(arguments.front());
    parse_arguments_(arguments, 1, true);
}

inline void flag_set::parse_file(const std::string& path, flag_file_format format) {
    if (flag_file_depth_ >= detail::max_flag_file_depth()) {
        fail_("flag file " + path + " is nested too deeply.");
    }

    std::string text;
    if (!detail::read_file_text(path, text)) {
        fail_("cannot open flag file " + path + ".");
    }
    if (format == flag_file_format::automatic) {
        format = detail::detect_flag_file_format(path, text);
    }

    std::vector<std::string> arguments;
    switch (format) {
        case flag_file_format::json:
            arguments = detail::json_flag_file_reader(path, text).read();
            break;
        case flag_file_format::yaml:
            arguments = detail::yaml_flag_file_reader(path, text).read();
            break;
        case flag_file_format::gflags:
        case flag_file_format::automatic:
            arguments = detail::read_gflags_flag_file(path, text);
            break;
    }

    const std::string saved_context = flag_file_context_;
    flag_file_context_ = "flag file " + path + ": ";
    ++flag_file_depth_;
    parse_arguments_(arguments, 0, false);
    --flag_file_depth_;
    flag_file_context_ = saved_context;
}

inline void flag_set::parse_arguments_(const std::vector<std::string>& arguments, std::size_t begin,
                                       bool allow_positional) {
    for (std::size_t index = begin; index < arguments.size(); ++index) {
        const std::string& segment = arguments[index];
        if (segment == "--") {
            if (!allow_positional) {
                fail_("positional arguments are not allowed here.");
            }
            args_.insert(args_.cend(), arguments.begin() + index + 1, arguments.end());
            break;
        }
        if (segment.compare(0, 3, "---") == 0) {
            fail_("invalid argument " + segment);
        }
        if (segment.size() > 2 && segment.compare(0, 2, "--") == 0) {
            parse_long_args_(segment, index, arguments);
            continue;
        }
        if (segment.size() > 1 && segment.front() == '-') {
            parse_short_args_(segment, index, arguments);
            continue;
        }
        if (!allow_positional) {
            fail_("invalid argument " + segment);
        }
        args_.push_back(segment);
    }
}

inline void flag_set::parse_long_args_(const std::string& segment, std::size_t& index,
                                       const std::vector<std::string>& arguments) {
    const std::string argument = segment.substr(2);
    const std::size_t separator = argument.find('=');
    const bool has_inline_value = separator != std::string::npos;
    const std::string flag_name = has_inline_value ? argument.substr(0, separator) : argument;
    std::string flag_value = has_inline_value ? argument.substr(separator + 1) : std::string();

    if (flag_name == detail::help_flag_name()) {
        usage();
        std::exit(EXIT_SUCCESS);
    }

    if (flag_name == detail::flag_file_flag_name()) {
        if (!has_inline_value) {
            if (index + 1 >= arguments.size()) {
                fail_("please set flag " + flag_name + " value.");
            }
            flag_value = arguments[++index];
        }
        parse_file(flag_value);
        return;
    }

    const std::shared_ptr<flag> flag = lookup_(flag_name, false);
    if (flag == nullptr) {
        fail_("flag " + flag_name + " not exist.");
    }

    const std::shared_ptr<value>& value = flag->value();
    if (value->has_implicit_value() && !has_inline_value) {
        flag_value = "true";
    } else if (!has_inline_value) {
        if (index + 1 >= arguments.size()) {
            fail_("please set flag " + flag_name + " value.");
        }
        flag_value = arguments[++index];
    }

    if (!value->set(flag_value)) {
        fail_("invalid value for " + flag_name + ".");
    }
}

inline void flag_set::parse_short_args_(const std::string& segment, std::size_t& index,
                                        const std::vector<std::string>& arguments) {
    const std::string argument = segment.substr(1);

    for (std::size_t argument_index = 0; argument_index < argument.size(); ++argument_index) {
        const std::string flag_name(1, argument[argument_index]);
        if (flag_name == detail::help_short_flag_name()) {
            usage();
            std::exit(EXIT_SUCCESS);
        }

        const std::shared_ptr<flag> flag = lookup_(flag_name, true);
        if (flag == nullptr) {
            fail_("flag " + flag_name + " not exist.");
        }

        const std::shared_ptr<value>& value = flag->value();
        std::string flag_value;
        if (value->has_implicit_value()) {
            flag_value = "true";
        } else if (argument_index == argument.size() - 1) {
            if (index + 1 >= arguments.size()) {
                fail_("please set flag " + flag_name + " value.");
            }
            flag_value = arguments[++index];
        } else {
            flag_value = argument.substr(argument_index + 1);
            argument_index = argument.size() - 1;
        }

        if (!value->set(flag_value)) {
            fail_("invalid value for " + flag_name + ".");
        }
    }
}

inline std::shared_ptr<flag> flag_set::lookup_(const std::string& name, bool short_name) const {
    const std::map<std::string, std::shared_ptr<flag>>& flags = short_name ? short_flags_ : flags_;
    const auto found = flags.find(name);
    return found == flags.end() ? nullptr : found->second;
}

inline void flag_set::add_flag_(const std::shared_ptr<flag>& flag) {
    const std::string& name = flag->name();
    const std::string& short_name = flag->short_name();

    if (name.empty() && short_name.empty()) {
        detail::fail("flag name cannot be empty.");
    }
    if (name == detail::help_flag_name() || short_name == detail::help_short_flag_name()) {
        detail::fail("flag name is reserved for help.");
    }
    if (name == detail::flag_file_flag_name()) {
        detail::fail("flag name is reserved for flag-file.");
    }
    if (!short_name.empty() && short_name.size() != 1) {
        detail::fail("short flag name must contain one character.");
    }
    if (!name.empty() && flags_.find(name) != flags_.end()) {
        detail::fail("redefine flag " + name + ".");
    }
    if (!short_name.empty() && short_flags_.find(short_name) != short_flags_.end()) {
        detail::fail("redefine flag " + short_name + ".");
    }

    if (!name.empty()) {
        flags_.emplace(name, flag);
    }
    if (!short_name.empty()) {
        short_flags_.emplace(short_name, flag);
    }
}

inline void flag_set::reset() {
    flags_.clear();
    short_flags_.clear();
    args_.clear();
    program_.clear();
    flag_file_depth_ = 0;
    flag_file_context_.clear();
}

inline void flag_set::fail_(const std::string& message) const { detail::fail(flag_file_context_ + message); }

namespace detail {

inline flag_set& global_flag_set_storage() {
    static flag_set flag_set;
    return flag_set;
}

}  // namespace detail

inline flag_set& global_flag_set() { return detail::global_flag_set_storage(); }

template <typename T>
inline void var(T* arg, const std::string& name, const typename detail::type_identity<T>::type& default_value,
                const std::string& usage_text) {
    detail::global_flag_set_storage().var(arg, name, default_value, usage_text);
}

template <typename T>
inline void varp(T* arg, const std::string& name, const std::string& short_name,
                 const typename detail::type_identity<T>::type& default_value, const std::string& usage_text) {
    detail::global_flag_set_storage().varp(arg, name, short_name, default_value, usage_text);
}

inline void parse(int argc, char* argv[]) { detail::global_flag_set_storage().parse(argc, argv); }

inline void parse(const std::vector<std::string>& arguments) { detail::global_flag_set_storage().parse(arguments); }

inline void parse_file(const std::string& path, flag_file_format format = flag_file_format::automatic) {
    detail::global_flag_set_storage().parse_file(path, format);
}

inline void reset() { detail::global_flag_set_storage().reset(); }

inline void usage() { detail::global_flag_set_storage().usage(); }

inline std::vector<std::string>& args() { return detail::global_flag_set_storage().args(); }

}  // namespace cflag
