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

TEST(test_bool, test_long_args_true) {
    bool result = false;
    std::vector<std::string> true_values{"TRUE", "True", "T", "true", "t", "1"};
    std::vector<std::string> arguments;

    for (auto &it : true_values) {
        cflag::reset();
        result = false;
        cflag::bool_var(&result, "test", false, "test true.");
        arguments.clear();
        arguments.push_back("test-true");
        std::string temp = "--test=" + it;
        arguments.push_back(temp);
        cflag::parse(arguments);
        EXPECT_TRUE(result);
    }
}

TEST(test_bool, test_long_args_false) {
    bool result = true;
    std::vector<std::string> true_values{"FALSE", "False", "F", "false", "f", "0"};
    std::vector<std::string> arguments;

    for (auto &it : true_values) {
        cflag::reset();
        result = true;
        cflag::bool_var(&result, "test", false, "test false.");
        arguments.clear();
        arguments.push_back("test-false");
        std::string temp = "--test=" + it;
        arguments.push_back(temp);
        cflag::parse(arguments);
        EXPECT_FALSE(result);
    }
}

TEST(test_bool, test_long_args_default_true) {
    bool result = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_var(&result, "test", false, "test true.");
    arguments.push_back("test-false");
    arguments.push_back("--test");
    cflag::parse(arguments);
    EXPECT_TRUE(result);
}

TEST(test_bool, test_short_args_default_true) {
    bool result = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_varp(&result, "test", "t", false, "test true.");
    arguments.push_back("test-true");
    std::string temp = "-t";
    arguments.push_back(temp);
    cflag::parse(arguments);
    EXPECT_TRUE(result);
}

TEST(test_bool, test_short_args_mixed) {
    bool result_00 = false;
    bool result_01 = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_varp(&result_00, "test00", "t", false, "test mixed.");
    cflag::bool_varp(&result_01, "test01", "u", false, "test mixed.");
    arguments.clear();
    arguments.push_back("test-mixed");
    arguments.push_back("-tu");
    cflag::parse(arguments);
    EXPECT_TRUE(result_00);
    EXPECT_TRUE(result_01);
}

TEST(test_bool, test_invalid) {
    bool result = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_var(&result, "test", false, "test invalid bool value.");
    arguments.clear();
    arguments.push_back("test-invalid");
    arguments.push_back("--test=invalid");
    EXPECT_EXIT(cflag::parse(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*invalid.*");
}

