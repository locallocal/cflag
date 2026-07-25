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

#include <cstdint>
#include <limits>

#include <gtest/gtest.h>

TEST(test_fixed_width, test_parse_boundaries) {
    std::uint32_t uint32_value = 0;
    std::int32_t int32_value = 0;
    std::uint64_t uint64_value = 0;
    std::int64_t int64_value = 0;
    const std::vector<std::string> arguments{
            "test-fixed-width",
            "--uint32=4294967295",
            "--int32=-2147483648",
            "--uint64=18446744073709551615",
            "--int64=-9223372036854775808"};

    cflag::reset();
    cflag::var(&uint32_value, "uint32", std::uint32_t{0}, "uint32 value.");
    cflag::var(&int32_value, "int32", std::int32_t{0}, "int32 value.");
    cflag::var(&uint64_value, "uint64", std::uint64_t{0}, "uint64 value.");
    cflag::var(&int64_value, "int64", std::int64_t{0}, "int64 value.");
    cflag::parse(arguments);

    EXPECT_EQ(std::numeric_limits<std::uint32_t>::max(), uint32_value);
    EXPECT_EQ(std::numeric_limits<std::int32_t>::min(), int32_value);
    EXPECT_EQ(std::numeric_limits<std::uint64_t>::max(), uint64_value);
    EXPECT_EQ(std::numeric_limits<std::int64_t>::min(), int64_value);
}

TEST(test_fixed_width, test_type_names) {
    EXPECT_EQ("uint32", cflag::flag_traits<std::uint32_t>::type_name());
    EXPECT_EQ("uint64", cflag::flag_traits<std::uint64_t>::type_name());
    EXPECT_EQ("int64", cflag::flag_traits<std::int64_t>::type_name());

    const std::string &int32_name =
            cflag::flag_traits<std::int32_t>::type_name();
    EXPECT_TRUE(int32_name == "int32" || int32_name == "int");
}

TEST(test_fixed_width, test_reject_uint32_overflow) {
    std::uint32_t result = 0;
    const std::vector<std::string> arguments{
            "test-fixed-width", "--test=4294967296"};

    cflag::reset();
    cflag::var(&result, "test", std::uint32_t{0}, "uint32 value.");

    EXPECT_EXIT(
            cflag::parse(arguments),
            testing::ExitedWithCode(EXIT_FAILURE),
            ".*invalid.*");
}

TEST(test_fixed_width, test_reject_negative_uint32) {
    std::uint32_t result = 0;
    const std::vector<std::string> arguments{"test-fixed-width", "--test=-1"};

    cflag::reset();
    cflag::var(&result, "test", std::uint32_t{0}, "uint32 value.");

    EXPECT_EXIT(
            cflag::parse(arguments),
            testing::ExitedWithCode(EXIT_FAILURE),
            ".*invalid.*");
}

TEST(test_fixed_width, test_reject_int32_overflow) {
    std::int32_t result = 0;
    const std::vector<std::string> arguments{
            "test-fixed-width", "--test=2147483648"};

    cflag::reset();
    cflag::var(&result, "test", std::int32_t{0}, "int32 value.");

    EXPECT_EXIT(
            cflag::parse(arguments),
            testing::ExitedWithCode(EXIT_FAILURE),
            ".*invalid.*");
}

TEST(test_fixed_width, test_reject_uint64_overflow) {
    std::uint64_t result = 0;
    const std::vector<std::string> arguments{
            "test-fixed-width", "--test=18446744073709551616"};

    cflag::reset();
    cflag::var(&result, "test", std::uint64_t{0}, "uint64 value.");

    EXPECT_EXIT(
            cflag::parse(arguments),
            testing::ExitedWithCode(EXIT_FAILURE),
            ".*invalid.*");
}

TEST(test_fixed_width, test_reject_int64_overflow) {
    std::int64_t result = 0;
    const std::vector<std::string> arguments{
            "test-fixed-width", "--test=9223372036854775808"};

    cflag::reset();
    cflag::var(&result, "test", std::int64_t{0}, "int64 value.");

    EXPECT_EXIT(
            cflag::parse(arguments),
            testing::ExitedWithCode(EXIT_FAILURE),
            ".*invalid.*");
}
