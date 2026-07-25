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
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace cflag {

class c_flag_set;
class c_flag;
class i_value;

namespace detail {

inline const std::string &bool_type_name() {
    static const std::string value = "bool";
    return value;
}

inline const std::string &int_type_name() {
    static const std::string value = "int";
    return value;
}

inline const std::string &int32_type_name() {
    static const std::string value = "int32";
    return value;
}

inline const std::string &uint32_type_name() {
    static const std::string value = "uint32";
    return value;
}

inline const std::string &int64_type_name() {
    static const std::string value = "int64";
    return value;
}

inline const std::string &uint64_type_name() {
    static const std::string value = "uint64";
    return value;
}

inline const std::string &char_type_name() {
    static const std::string value = "char";
    return value;
}

inline const std::string &signed_char_type_name() {
    static const std::string value = "signed char";
    return value;
}

inline const std::string &unsigned_char_type_name() {
    static const std::string value = "unsigned char";
    return value;
}

inline const std::string &wchar_type_name() {
    static const std::string value = "wchar_t";
    return value;
}

inline const std::string &char16_type_name() {
    static const std::string value = "char16_t";
    return value;
}

inline const std::string &char32_type_name() {
    static const std::string value = "char32_t";
    return value;
}

#if defined(__cpp_char8_t)
inline const std::string &char8_type_name() {
    static const std::string value = "char8_t";
    return value;
}
#endif

inline const std::string &short_type_name() {
    static const std::string value = "short";
    return value;
}

inline const std::string &unsigned_short_type_name() {
    static const std::string value = "unsigned short";
    return value;
}

inline const std::string &unsigned_int_type_name() {
    static const std::string value = "unsigned int";
    return value;
}

inline const std::string &long_type_name() {
    static const std::string value = "long";
    return value;
}

inline const std::string &unsigned_long_type_name() {
    static const std::string value = "unsigned long";
    return value;
}

inline const std::string &long_long_type_name() {
    static const std::string value = "long long";
    return value;
}

inline const std::string &unsigned_long_long_type_name() {
    static const std::string value = "unsigned long long";
    return value;
}

inline const std::string &float_type_name() {
    static const std::string value = "float";
    return value;
}

inline const std::string &double_type_name() {
    static const std::string value = "double";
    return value;
}

inline const std::string &long_double_type_name() {
    static const std::string value = "long double";
    return value;
}

inline const std::string &nullptr_type_name() {
    static const std::string value = "nullptr";
    return value;
}

inline const std::string &string_type_name() {
    static const std::string value = "string";
    return value;
}

inline const std::string &help_flag_name() {
    static const std::string value = "help";
    return value;
}

inline const std::string &help_short_flag_name() {
    static const std::string value = "h";
    return value;
}

template <typename>
struct dependent_false : std::false_type {};

template <typename T>
struct type_identity {
    typedef T type;
};

template <typename T>
struct is_supported_integer
    : std::integral_constant<
              bool,
              std::is_integral<T>::value &&
                      !std::is_same<T, bool>::value> {};

template <typename T>
struct floating_parser;

template <>
struct floating_parser<float> {
    static float parse(const std::string &value, std::size_t *parsed_length) {
        return std::stof(value, parsed_length);
    }
};

template <>
struct floating_parser<double> {
    static double parse(const std::string &value, std::size_t *parsed_length) {
        return std::stod(value, parsed_length);
    }
};

template <>
struct floating_parser<long double> {
    static long double parse(
            const std::string &value,
            std::size_t *parsed_length) {
        return std::stold(value, parsed_length);
    }
};

[[noreturn]] inline void fail(const std::string &message) {
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
}

} // namespace detail

// Specialize flag_traits for a custom type to use it with var<T>/varp<T>.
template <typename T, typename Enable = void>
struct flag_traits {
    static_assert(
            detail::dependent_false<T>::value,
            "cflag::flag_traits<T> must be specialized for this flag type");
};

template <>
struct flag_traits<bool> {
    static const std::string &type_name() {
        return detail::bool_type_name();
    }

    static std::string format(bool value) {
        return value ? "true" : "false";
    }

    static bool parse(const std::string &value, bool &output) {
        if (value == "TRUE" || value == "True" || value == "T" || value == "true" ||
                value == "t" || value == "1") {
            output = true;
            return true;
        }
        if (value == "FALSE" || value == "False" || value == "F" || value == "false" ||
                value == "f" || value == "0") {
            output = false;
            return true;
        }
        return false;
    }

    static bool has_implicit_value() {
        return true;
    }
};

template <typename T>
struct flag_traits<
        T,
        typename std::enable_if<detail::is_supported_integer<T>::value>::type> {
    static const std::string &type_name() {
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

    static std::string format(T value) {
        return std::to_string(value);
    }

    static bool parse(const std::string &value, T &output) {
        return parse_value(
                value,
                output,
                std::integral_constant<bool, std::numeric_limits<T>::is_signed>());
    }

    static bool has_implicit_value() {
        return false;
    }

private:
    static bool parse_value(
            const std::string &value,
            T &output,
            std::true_type) {
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
        } catch (const std::invalid_argument &) {
            return false;
        } catch (const std::out_of_range &) {
            return false;
        }
    }

    static bool parse_value(
            const std::string &value,
            T &output,
            std::false_type) {
        const std::size_t first_character =
                value.find_first_not_of(" \t\n\r\f\v");
        if (first_character == std::string::npos || value[first_character] == '-') {
            return false;
        }

        try {
            std::size_t parsed_length = 0;
            const unsigned long long parsed_value =
                    std::stoull(value, &parsed_length);
            if (parsed_length != value.size()) {
                return false;
            }
            if (parsed_value >
                    static_cast<unsigned long long>(std::numeric_limits<T>::max())) {
                return false;
            }
            output = static_cast<T>(parsed_value);
            return true;
        } catch (const std::invalid_argument &) {
            return false;
        } catch (const std::out_of_range &) {
            return false;
        }
    }
};

template <typename T>
struct flag_traits<
        T,
        typename std::enable_if<std::is_floating_point<T>::value>::type> {
    static const std::string &type_name() {
        if (std::is_same<T, float>::value) {
            return detail::float_type_name();
        }
        if (std::is_same<T, double>::value) {
            return detail::double_type_name();
        }
        return detail::long_double_type_name();
    }

    static std::string format(T value) {
        return std::to_string(value);
    }

    static bool parse(const std::string &value, T &output) {
        try {
            std::size_t parsed_length = 0;
            const T parsed_value =
                    detail::floating_parser<T>::parse(value, &parsed_length);
            if (parsed_length != value.size()) {
                return false;
            }
            output = parsed_value;
            return true;
        } catch (const std::invalid_argument &) {
            return false;
        } catch (const std::out_of_range &) {
            return false;
        }
    }

    static bool has_implicit_value() {
        return false;
    }
};

template <>
struct flag_traits<std::nullptr_t> {
    static const std::string &type_name() {
        return detail::nullptr_type_name();
    }

    static std::string format(std::nullptr_t) {
        return "nullptr";
    }

    static bool parse(const std::string &value, std::nullptr_t &output) {
        if (value != "nullptr") {
            return false;
        }
        output = nullptr;
        return true;
    }

    static bool has_implicit_value() {
        return false;
    }
};

template <>
struct flag_traits<std::string> {
    static const std::string &type_name() {
        return detail::string_type_name();
    }

    static const std::string &format(const std::string &value) {
        return value;
    }

    static bool parse(const std::string &value, std::string &output) {
        output = value;
        return true;
    }

    static bool has_implicit_value() {
        return false;
    }
};

class i_value {
public:
    virtual ~i_value() = default;
    virtual bool set(const std::string &value) = 0;
    virtual const std::string &type() const = 0;
    virtual bool has_implicit_value() const = 0;
};

template <typename T>
class c_value final : public i_value {
public:
    explicit c_value(T *arg) : arg_(arg) {}

    bool set(const std::string &value) override {
        if (arg_ == nullptr) {
            return false;
        }
        T parsed_value = *arg_;
        if (!flag_traits<T>::parse(value, parsed_value)) {
            return false;
        }
        *arg_ = parsed_value;
        return true;
    }

    const std::string &type() const override {
        return flag_traits<T>::type_name();
    }

    bool has_implicit_value() const override {
        return flag_traits<T>::has_implicit_value();
    }

private:
    T *arg_;
};

class c_flag {
public:
    explicit c_flag(const std::string &name) : name_(name) {}

    void short_name(const std::string &short_name) {
        short_name_ = short_name;
    }

    void usage(const std::string &usage) {
        usage_ = usage;
    }

    void default_value(const std::string &default_value) {
        default_value_ = default_value;
    }

    void value(const std::shared_ptr<i_value> &value) {
        value_ = value;
    }

    const std::string &name() const {
        return name_;
    }

    const std::string &usage() const {
        return usage_;
    }

    const std::string &short_name() const {
        return short_name_;
    }

    const std::string &default_value() const {
        return default_value_;
    }

    const std::shared_ptr<i_value> &value() const {
        return value_;
    }

private:
    std::string name_;
    std::string short_name_;
    std::string usage_;
    std::string default_value_;
    std::shared_ptr<i_value> value_;
};

class c_flag_set {
public:
    c_flag_set() = default;
    c_flag_set(const c_flag_set &) = delete;
    c_flag_set(c_flag_set &&) = delete;
    c_flag_set &operator=(const c_flag_set &) = delete;
    c_flag_set &operator=(c_flag_set &&) = delete;

    void usage() const;
    void print_flags() const;
    void parse(int argc, char *argv[]);
    void parse(const std::vector<std::string> &arguments);
    void reset();

    template <typename T>
    void var(
            T *arg,
            const std::string &name,
            const typename detail::type_identity<T>::type &default_value,
            const std::string &usage);

    template <typename T>
    void varp(
            T *arg,
            const std::string &name,
            const std::string &short_name,
            const typename detail::type_identity<T>::type &default_value,
            const std::string &usage);

    void program(const std::string &value) {
        program_ = value;
    }

    const std::string &program() const {
        return program_;
    }

    std::vector<std::string> &args() {
        return args_;
    }

    const std::vector<std::string> &args() const {
        return args_;
    }

private:
    std::shared_ptr<c_flag> lookup_(const std::string &name, bool short_name) const;
    void add_flag_(const std::shared_ptr<c_flag> &flag);
    void parse_long_args_(
            const std::string &segment,
            std::size_t &index,
            const std::vector<std::string> &arguments);
    void parse_short_args_(
            const std::string &segment,
            std::size_t &index,
            const std::vector<std::string> &arguments);

    std::string program_;
    std::map<std::string, std::shared_ptr<c_flag>> flags_;
    std::map<std::string, std::shared_ptr<c_flag>> short_flags_;
    std::vector<std::string> args_;
};

template <typename T>
inline void c_flag_set::var(
        T *arg,
        const std::string &name,
        const typename detail::type_identity<T>::type &default_value,
        const std::string &usage_text) {
    varp(arg, name, "", default_value, usage_text);
}

template <typename T>
inline void c_flag_set::varp(
        T *arg,
        const std::string &name,
        const std::string &short_name,
        const typename detail::type_identity<T>::type &default_value,
        const std::string &usage_text) {
    if (arg == nullptr) {
        detail::fail("flag target cannot be null.");
    }

    std::shared_ptr<c_flag> flag = std::make_shared<c_flag>(name);
    flag->short_name(short_name);
    flag->usage(usage_text);
    flag->default_value(flag_traits<T>::format(default_value));
    flag->value(std::make_shared<c_value<T>>(arg));

    add_flag_(flag);
    *arg = default_value;
}

inline void c_flag_set::usage() const {
    std::cout << "Usage: " << program() << " [options]\n\n";
    print_flags();
}

inline void c_flag_set::print_flags() const {
    std::vector<std::string> flag_labels;
    flag_labels.reserve(flags_.size());
    std::size_t label_width = 0;

    for (const auto &entry : flags_) {
        const std::shared_ptr<c_flag> &flag = entry.second;
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

    std::size_t index = 0;
    for (const auto &entry : flags_) {
        const std::shared_ptr<c_flag> &flag = entry.second;
        const std::string &label = flag_labels[index++];
        std::cout << label << std::string(label_width - label.size() + 1, ' ')
                  << flag->usage();
        if (!flag->default_value().empty()) {
            std::cout << '(' << flag->default_value() << ')';
        }
        std::cout << '\n';
    }
}

inline void c_flag_set::parse(int argc, char *argv[]) {
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

inline void c_flag_set::parse(const std::vector<std::string> &arguments) {
    args_.clear();
    if (arguments.empty()) {
        program_.clear();
        return;
    }

    program(arguments.front());
    for (std::size_t index = 1; index < arguments.size(); ++index) {
        const std::string &segment = arguments[index];
        if (segment == "--") {
            args_.insert(args_.cend(), arguments.begin() + index + 1, arguments.end());
            break;
        }
        if (segment.compare(0, 3, "---") == 0) {
            detail::fail("invalid argument " + segment);
        }
        if (segment.size() > 2 && segment.compare(0, 2, "--") == 0) {
            parse_long_args_(segment, index, arguments);
            continue;
        }
        if (segment.size() > 1 && segment.front() == '-') {
            parse_short_args_(segment, index, arguments);
            continue;
        }
        args_.push_back(segment);
    }
}

inline void c_flag_set::parse_long_args_(
        const std::string &segment,
        std::size_t &index,
        const std::vector<std::string> &arguments) {
    const std::string argument = segment.substr(2);
    const std::size_t separator = argument.find('=');
    const bool has_inline_value = separator != std::string::npos;
    const std::string flag_name =
            has_inline_value ? argument.substr(0, separator) : argument;
    std::string flag_value =
            has_inline_value ? argument.substr(separator + 1) : std::string();

    if (flag_name == detail::help_flag_name()) {
        usage();
        std::exit(EXIT_SUCCESS);
    }

    const std::shared_ptr<c_flag> flag = lookup_(flag_name, false);
    if (flag == nullptr) {
        detail::fail("flag " + flag_name + " not exist.");
    }

    const std::shared_ptr<i_value> &value = flag->value();
    if (value->has_implicit_value() && !has_inline_value) {
        flag_value = "true";
    } else if (!has_inline_value) {
        if (index + 1 >= arguments.size()) {
            detail::fail("please set flag " + flag_name + " value.");
        }
        flag_value = arguments[++index];
    }

    if (!value->set(flag_value)) {
        detail::fail("invalid value for " + flag_name + ".");
    }
}

inline void c_flag_set::parse_short_args_(
        const std::string &segment,
        std::size_t &index,
        const std::vector<std::string> &arguments) {
    const std::string argument = segment.substr(1);

    for (std::size_t argument_index = 0; argument_index < argument.size(); ++argument_index) {
        const std::string flag_name(1, argument[argument_index]);
        if (flag_name == detail::help_short_flag_name()) {
            usage();
            std::exit(EXIT_SUCCESS);
        }

        const std::shared_ptr<c_flag> flag = lookup_(flag_name, true);
        if (flag == nullptr) {
            detail::fail("flag " + flag_name + " not exist.");
        }

        const std::shared_ptr<i_value> &value = flag->value();
        std::string flag_value;
        if (value->has_implicit_value()) {
            flag_value = "true";
        } else if (argument_index == argument.size() - 1) {
            if (index + 1 >= arguments.size()) {
                detail::fail("please set flag " + flag_name + " value.");
            }
            flag_value = arguments[++index];
        } else {
            flag_value = argument.substr(argument_index + 1);
            argument_index = argument.size() - 1;
        }

        if (!value->set(flag_value)) {
            detail::fail("invalid value for " + flag_name + ".");
        }
    }
}

inline std::shared_ptr<c_flag> c_flag_set::lookup_(
        const std::string &name,
        bool short_name) const {
    const std::map<std::string, std::shared_ptr<c_flag>> &flags =
            short_name ? short_flags_ : flags_;
    const auto found = flags.find(name);
    return found == flags.end() ? nullptr : found->second;
}

inline void c_flag_set::add_flag_(const std::shared_ptr<c_flag> &flag) {
    const std::string &name = flag->name();
    const std::string &short_name = flag->short_name();

    if (name.empty() && short_name.empty()) {
        detail::fail("flag name cannot be empty.");
    }
    if (name == detail::help_flag_name() || short_name == detail::help_short_flag_name()) {
        detail::fail("flag name is reserved for help.");
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

inline void c_flag_set::reset() {
    flags_.clear();
    short_flags_.clear();
    args_.clear();
    program_.clear();
}

namespace detail {

inline c_flag_set &global_flag_set_storage() {
    static c_flag_set flag_set;
    return flag_set;
}

} // namespace detail

inline c_flag_set &global_flag_set() {
    return detail::global_flag_set_storage();
}

template <typename T>
inline void var(
        T *arg,
        const std::string &name,
        const typename detail::type_identity<T>::type &default_value,
        const std::string &usage_text) {
    detail::global_flag_set_storage().var(arg, name, default_value, usage_text);
}

template <typename T>
inline void varp(
        T *arg,
        const std::string &name,
        const std::string &short_name,
        const typename detail::type_identity<T>::type &default_value,
        const std::string &usage_text) {
    detail::global_flag_set_storage().varp(
            arg, name, short_name, default_value, usage_text);
}

inline void parse(int argc, char *argv[]) {
    detail::global_flag_set_storage().parse(argc, argv);
}

inline void parse(const std::vector<std::string> &arguments) {
    detail::global_flag_set_storage().parse(arguments);
}

inline void reset() {
    detail::global_flag_set_storage().reset();
}

inline void usage() {
    detail::global_flag_set_storage().usage();
}

inline std::vector<std::string> &args() {
    return detail::global_flag_set_storage().args();
}

} // namespace cflag
