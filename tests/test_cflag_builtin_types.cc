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

#include "cflag.h"

#include <cstddef>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace {

std::string increment_decimal(std::string value) {
    for (std::string::reverse_iterator digit = value.rbegin();
            digit != value.rend();
            ++digit) {
        if (*digit != '9') {
            ++*digit;
            return value;
        }
        *digit = '0';
    }
    return "1" + value;
}

template <typename T>
void expect_flag_value(const std::string &text, T expected) {
    T result = T();
    const std::vector<std::string> arguments{
            "test-builtin-types", "--value=" + text};

    cflag::reset();
    cflag::var(&result, "value", T(), "built-in value.");
    cflag::parse(arguments);

    EXPECT_EQ(expected, result);
}

template <typename T>
void expect_integer_boundaries(T, std::true_type) {
    T result = T();
    const long long minimum =
            static_cast<long long>(std::numeric_limits<T>::min());
    const long long maximum =
            static_cast<long long>(std::numeric_limits<T>::max());

    EXPECT_TRUE(cflag::flag_traits<T>::parse(std::to_string(minimum), result));
    EXPECT_EQ(std::numeric_limits<T>::min(), result);
    EXPECT_TRUE(cflag::flag_traits<T>::parse(std::to_string(maximum), result));
    EXPECT_EQ(std::numeric_limits<T>::max(), result);

    const unsigned long long minimum_magnitude =
            static_cast<unsigned long long>(-(minimum + 1)) + 1;
    EXPECT_FALSE(cflag::flag_traits<T>::parse(
            "-" + increment_decimal(std::to_string(minimum_magnitude)),
            result));
    EXPECT_FALSE(cflag::flag_traits<T>::parse(
            increment_decimal(std::to_string(maximum)),
            result));
}

template <typename T>
void expect_integer_boundaries(T, std::false_type) {
    T result = T();
    const unsigned long long maximum =
            static_cast<unsigned long long>(std::numeric_limits<T>::max());

    EXPECT_TRUE(cflag::flag_traits<T>::parse("0", result));
    EXPECT_EQ(T(), result);
    EXPECT_TRUE(cflag::flag_traits<T>::parse(std::to_string(maximum), result));
    EXPECT_EQ(std::numeric_limits<T>::max(), result);

    EXPECT_FALSE(cflag::flag_traits<T>::parse(
            increment_decimal(std::to_string(maximum)),
            result));
    EXPECT_FALSE(cflag::flag_traits<T>::parse("-1", result));
}

template <typename T>
void expect_integer_boundaries() {
    expect_integer_boundaries(T(), std::is_signed<T>());
}

} // namespace

TEST(test_builtin_types, test_character_types) {
    expect_flag_value<char>("65", static_cast<char>(65));
    expect_flag_value<signed char>("-12", static_cast<signed char>(-12));
    expect_flag_value<unsigned char>("240", static_cast<unsigned char>(240));
    expect_flag_value<wchar_t>("66", static_cast<wchar_t>(66));
    expect_flag_value<char16_t>("67", static_cast<char16_t>(67));
    expect_flag_value<char32_t>("68", static_cast<char32_t>(68));
#if defined(__cpp_char8_t)
    expect_flag_value<char8_t>("69", static_cast<char8_t>(69));
#endif
}

TEST(test_builtin_types, test_integer_types) {
    expect_flag_value<short>("-123", static_cast<short>(-123));
    expect_flag_value<unsigned short>("456", static_cast<unsigned short>(456));
    expect_flag_value<int>("-789", -789);
    expect_flag_value<unsigned int>("789", 789U);
    expect_flag_value<long>("-1234", -1234L);
    expect_flag_value<unsigned long>("1234", 1234UL);
    expect_flag_value<long long>("-5678", -5678LL);
    expect_flag_value<unsigned long long>("5678", 5678ULL);
}

TEST(test_builtin_types, test_floating_point_types) {
    expect_flag_value<float>("1.25", 1.25F);
    expect_flag_value<double>("-2.5", -2.5);
    expect_flag_value<long double>("3.75", 3.75L);

    float float_value = 0.0F;
    double double_value = 0.0;
    long double long_double_value = 0.0L;
    EXPECT_FALSE(cflag::flag_traits<float>::parse("1.0f", float_value));
    EXPECT_FALSE(cflag::flag_traits<double>::parse("2.0d", double_value));
    EXPECT_FALSE(
            cflag::flag_traits<long double>::parse("3.0L", long_double_value));
}

TEST(test_builtin_types, test_nullptr_type) {
    expect_flag_value<std::nullptr_t>("nullptr", nullptr);

    std::nullptr_t result = nullptr;
    EXPECT_FALSE(cflag::flag_traits<std::nullptr_t>::parse("null", result));
    EXPECT_EQ("nullptr", cflag::flag_traits<std::nullptr_t>::format(result));
}

TEST(test_builtin_types, test_unambiguous_type_names) {
    EXPECT_EQ("char", cflag::flag_traits<char>::type_name());
    EXPECT_EQ("signed char", cflag::flag_traits<signed char>::type_name());
    EXPECT_EQ("unsigned char", cflag::flag_traits<unsigned char>::type_name());
    EXPECT_EQ("wchar_t", cflag::flag_traits<wchar_t>::type_name());
    EXPECT_EQ("char16_t", cflag::flag_traits<char16_t>::type_name());
    EXPECT_EQ("char32_t", cflag::flag_traits<char32_t>::type_name());
#if defined(__cpp_char8_t)
    EXPECT_EQ("char8_t", cflag::flag_traits<char8_t>::type_name());
#endif
    EXPECT_EQ("float", cflag::flag_traits<float>::type_name());
    EXPECT_EQ("double", cflag::flag_traits<double>::type_name());
    EXPECT_EQ("long double", cflag::flag_traits<long double>::type_name());
    EXPECT_EQ("nullptr", cflag::flag_traits<std::nullptr_t>::type_name());
}

TEST(test_builtin_types, test_all_integer_boundaries) {
    expect_integer_boundaries<char>();
    expect_integer_boundaries<signed char>();
    expect_integer_boundaries<unsigned char>();
    expect_integer_boundaries<wchar_t>();
    expect_integer_boundaries<char16_t>();
    expect_integer_boundaries<char32_t>();
#if defined(__cpp_char8_t)
    expect_integer_boundaries<char8_t>();
#endif
    expect_integer_boundaries<short>();
    expect_integer_boundaries<unsigned short>();
    expect_integer_boundaries<int>();
    expect_integer_boundaries<unsigned int>();
    expect_integer_boundaries<long>();
    expect_integer_boundaries<unsigned long>();
    expect_integer_boundaries<long long>();
    expect_integer_boundaries<unsigned long long>();
}
