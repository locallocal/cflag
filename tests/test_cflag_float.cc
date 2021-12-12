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

TEST(test_float, test_long_args_float) {
    float result = 0.0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::float_var(&result, "test", 0.0, "test float.");
    arguments.clear();
    arguments.push_back("test-float");
    arguments.push_back("--test=1.999");
    cflag::parse(arguments);
    EXPECT_FLOAT_EQ(1.999f, result);
}

TEST(test_float, test_short_args_float) {
    float result = 0.0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::float_varp(&result, "test", "t", 0.0, "test float.");
    arguments.clear();
    arguments.push_back("test-float");
    arguments.push_back("-t 1.999");
    cflag::parse(arguments);
    EXPECT_FLOAT_EQ(1.999f, result);
}

TEST(test_float, test_short_args_float_mixed) {
    float result = 0.0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::float_varp(&result, "test", "t", 0.0, "test float.");
    arguments.clear();
    arguments.push_back("test-float");
    arguments.push_back("-t1.999");
    cflag::parse(arguments);
    EXPECT_FLOAT_EQ(1.999f, result);
}

TEST(test_float, test_float_default) {
    float result = 0.0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::float_var(&result, "test", 1.999, "test float.");
    EXPECT_FLOAT_EQ(1.999f, result);
}

TEST(test_float, test_float_invalid) {
    float result = 0.0;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::float_var(&result, "test", 0.0, "test invalid float.");
    arguments.clear();
    arguments.push_back("test-float");
    arguments.push_back("--test=hello");
    EXPECT_EXIT(cflag::parse(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*invalid.*");
}
