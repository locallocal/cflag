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

TEST(test_common, test_usage) {
    bool result;
    std::vector<std::string> arguments;
    
    cflag::reset();
    cflag::bool_varp(&result, "test", "t", true, "test usage.");
    arguments.push_back("test-common");
    cflag::parse(arguments);
    cflag::usage();
    EXPECT_TRUE(result);
}

TEST(test_common, test_terminat) {
    bool result_00 = false;
    bool result_01 = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_var(&result_00, "test00", false, "first bool flag.");
    cflag::bool_var(&result_01, "test01", false, "second bool flag.");
    arguments.clear();
    arguments.push_back("test-terminate");
    arguments.push_back("--test00=true");
    arguments.push_back("--");
    arguments.push_back("--test01=true");
    cflag::parse(arguments);

    EXPECT_TRUE(result_00);
    EXPECT_FALSE(result_01);
}

TEST(test_common, test_exist_args) {
    bool result = false;
    std::string arg00 = "arg00";
    std::string arg01 = "arg01";
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_var(&result, "test", false, "test args.");
    arguments.clear();
    arguments.push_back("test-args");
    arguments.push_back("--test=true");
    arguments.push_back("--");
    arguments.push_back("arg00");
    arguments.push_back("arg01");
    cflag::parse(arguments);

    EXPECT_TRUE(result);
    
    std::vector<std::string> &args = cflag::args();
    EXPECT_EQ(2, args.size());
    EXPECT_STREQ(args.at(0).c_str(), arg00.c_str());
    EXPECT_STREQ(args.at(1).c_str(), arg01.c_str());
}

TEST(test_common, test_left_args) {
    bool result = false;
    std::string arg00 = "arg00";
    std::string arg01 = "arg01";
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_var(&result, "test", false, "test args.");
    arguments.clear();
    arguments.push_back("test-args");
    arguments.push_back("--test=true");
    arguments.push_back("arg00");
    arguments.push_back("arg01");
    cflag::parse(arguments);

    EXPECT_TRUE(result);

    std::vector<std::string> &args = cflag::args();
    EXPECT_EQ(2, args.size());
    EXPECT_STREQ(arg00.c_str(), args.at(0).c_str());
    EXPECT_STREQ(arg01.c_str(), args.at(1).c_str());
}

TEST(test_common, test_null_args) {
    bool result = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::bool_var(&result, "test", false, "test args.");
    arguments.clear();
    arguments.push_back("test-args");
    arguments.push_back("--test=true");
    arguments.push_back("--");
    cflag::parse(arguments);

    EXPECT_TRUE(result);
    
    std::vector<std::string> &args = cflag::args();
    EXPECT_EQ(0, args.size());
}
