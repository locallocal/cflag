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

TEST(test_common, test_empty_argument_list) {
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::parse(arguments);

    EXPECT_TRUE(cflag::args().empty());
}

TEST(test_common, test_empty_and_single_dash_are_positional_args) {
    std::vector<std::string> arguments{"test-args", "", "-"};

    cflag::reset();
    cflag::parse(arguments);

    ASSERT_EQ(2, cflag::args().size());
    EXPECT_EQ("", cflag::args()[0]);
    EXPECT_EQ("-", cflag::args()[1]);
}

TEST(test_common, test_parse_replaces_positional_args) {
    std::vector<std::string> first_arguments{"test-args", "first"};
    std::vector<std::string> second_arguments{"test-args", "second"};

    cflag::reset();
    cflag::parse(first_arguments);
    cflag::parse(second_arguments);

    ASSERT_EQ(1, cflag::args().size());
    EXPECT_EQ("second", cflag::args()[0]);
}

TEST(test_common, test_short_flag_requires_value) {
    int result = 0;
    std::vector<std::string> arguments{"test-args", "-t"};

    cflag::reset();
    cflag::int_varp(&result, "test", "t", 0, "test missing value.");

    EXPECT_EXIT(cflag::parse(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*set flag.*value.*");
}

TEST(test_common, test_long_and_short_names_have_separate_lookups) {
    bool long_result = false;
    bool short_result = false;
    std::vector<std::string> arguments{"test-args", "--t", "-t"};

    cflag::reset();
    cflag::bool_var(&long_result, "t", false, "long flag.");
    cflag::bool_varp(&short_result, "test", "t", false, "short flag.");
    cflag::parse(arguments);

    EXPECT_TRUE(long_result);
    EXPECT_TRUE(short_result);
}

TEST(test_common, test_rejects_multi_character_short_name) {
    bool result = false;

    cflag::reset();

    EXPECT_EXIT(
            cflag::bool_varp(&result, "test", "tt", false, "invalid short flag."),
            testing::ExitedWithCode(EXIT_FAILURE),
            ".*one character.*");
}
