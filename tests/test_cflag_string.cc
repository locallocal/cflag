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
#include <gtest/gtest.h>

TEST(test_string, test_long_args_string) {
    std::string result;
    std::string expect = "hello";
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::string_var(&result, "test", "", "test string.");
    arguments.clear();
    arguments.push_back("test-string");
    arguments.push_back("--test=hello");
    cflag::parse(arguments);
    EXPECT_STREQ(expect.c_str(), result.c_str());
}

TEST(test_string, test_short_args_string) {
    std::string result;
    std::string expect = "hello";
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::string_varp(&result, "test", "t", "", "test string.");
    arguments.clear();
    arguments.push_back("test-string");
    arguments.push_back("-t");
    arguments.push_back("hello");
    cflag::parse(arguments);
    EXPECT_STREQ(expect.c_str(), result.c_str());
}

TEST(test_string, test_short_args_mixed_string) {
    std::string result;
    std::string expect = "hello";
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::string_varp(&result, "test", "t", "", "test string.");
    arguments.clear();
    arguments.push_back("test-string");
    arguments.push_back("-thello");
    cflag::parse(arguments);
    EXPECT_STREQ(expect.c_str(), result.c_str());
}
