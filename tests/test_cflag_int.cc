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

TEST(test_int, test_long_args_positive_int) {
    int result = 0;
    std::vector<std::string> arguments;
    
    cflag::reset();
    cflag::int_var(&result, "test", 0, "test int.");
    arguments.clear();
    arguments.push_back("test-int");
    arguments.push_back("--test=1");
    cflag::parse(arguments);
    EXPECT_EQ(1, result);
}

TEST(test_int, test_long_args_negative_int) {
    int result = 0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::int_var(&result, "test", 0, "test int.");
    arguments.clear();
    arguments.push_back("test-int");
    arguments.push_back("--test=-1");
    cflag::parse(arguments);
    EXPECT_EQ(-1, result);
}

TEST(test_int, test_short_args_positive_int) {
    int result = 0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::int_varp(&result, "test", "t", 0, "test int.");
    arguments.clear();
    arguments.push_back("test-int");
    arguments.push_back("-t");
    arguments.push_back("1");
    cflag::parse(arguments);
    EXPECT_EQ(1, result);
}

TEST(test_int, test_short_args_negtive_int) {
    int result = 0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::int_varp(&result, "test", "t", 0, "test int.");
    arguments.clear();
    arguments.push_back("test-int");
    arguments.push_back("-t");
    arguments.push_back("-1");
    cflag::parse(arguments);
    EXPECT_EQ(-1, result);
}

TEST(test_int, test_short_args_positive_int_mixed) {
    int result = 0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::int_varp(&result, "test", "t", 0, "test int.");
    arguments.clear();
    arguments.push_back("test-int");
    arguments.push_back("-t1");
    cflag::parse(arguments);
    EXPECT_EQ(1, result);
}

TEST(test_int, test_short_args_negative_int_mixed) {
    int result = 0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::int_varp(&result, "test", "t", 0, "test int.");
    arguments.clear();
    arguments.push_back("test-int");
    arguments.push_back("-t-1");
    cflag::parse(arguments);
    EXPECT_EQ(-1, result);
}

TEST(test_int, test_invalid) {
    int result = 0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::int_var(&result, "test", 0, "test invalid int value.");
    arguments.push_back("test-int");
    arguments.push_back("--test=invalid");
    EXPECT_EXIT(cflag::parse(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*invalid.*");
}
