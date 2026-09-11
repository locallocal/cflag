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

#include <gtest/gtest.h>

#include "cflag.h"

TEST(test_common, test_usage) {
    bool result;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::varp(&result, "test", "t", true, "test usage.");
    arguments.push_back("test-common");
    cflag::parse(arguments);
    cflag::usage();
    EXPECT_TRUE(result);
}

TEST(test_common, test_print_flags_aligns_usage_column) {
    bool alpha = false;
    std::string long_option;
    cflag::flag_set flag_set;

    flag_set.varp(&alpha, "alpha", "a", false, "alpha usage.");
    flag_set.var(&long_option, "long-option", std::string(), "long option usage.");

    testing::internal::CaptureStdout();
    flag_set.print_flags();
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(
        " -a  --alpha[bool]         alpha usage.(false)\n"
        "     --long-option[string] long option usage.\n",
        output);
}

TEST(test_common, test_print_flags_wraps_long_usage) {
    std::string option;
    cflag::flag_set flag_set;

    flag_set.var(&option, "option", std::string("default"),
                 "aaaa bbbb cccc dddd eeee ffff gggg hhhh iiii jjjj kkkk llll mmmm nnnn oooo pppp qqqq rrrr ssss "
                 "tttt uuuu vvvv wwww xxxx yyyy zzzz");

    testing::internal::CaptureStdout();
    flag_set.print_flags();
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(
        "     --option[string] aaaa bbbb cccc dddd eeee ffff gggg hhhh iiii jjjj kkkk llll mmmm nnnn oooo\n"
        "                      pppp qqqq rrrr ssss tttt uuuu vvvv wwww xxxx yyyy zzzz(default)\n",
        output);
}

TEST(test_common, test_print_flags_limits_line_width) {
    int number = 0;
    std::string word;
    cflag::flag_set flag_set;

    flag_set.varp(&number, "number", "n", 42,
                  "a fairly long description that keeps going on and on so that it needs to be wrapped over "
                  "several lines before the output fits into the width limit.");
    flag_set.var(&word, "word", std::string(),
                 std::string("no-spaces-") + std::string(120, 'x') + " tail of the description.");

    testing::internal::CaptureStdout();
    flag_set.print_flags();
    const std::string output = testing::internal::GetCapturedStdout();

    std::size_t line_count = 0;
    std::size_t begin = 0;
    while (begin < output.size()) {
        std::size_t end = output.find('\n', begin);
        if (end == std::string::npos) {
            end = output.size();
        }
        EXPECT_LE(end - begin, 100u) << output.substr(begin, end - begin);
        ++line_count;
        begin = end + 1;
    }
    EXPECT_GT(line_count, 2u);
    EXPECT_NE(output.find("(42)"), std::string::npos);
    EXPECT_NE(output.find("tail of the description."), std::string::npos);
}

TEST(test_common, test_terminat) {
    bool result_00 = false;
    bool result_01 = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::var(&result_00, "test00", false, "first bool flag.");
    cflag::var(&result_01, "test01", false, "second bool flag.");
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
    cflag::var(&result, "test", false, "test args.");
    arguments.clear();
    arguments.push_back("test-args");
    arguments.push_back("--test=true");
    arguments.push_back("--");
    arguments.push_back("arg00");
    arguments.push_back("arg01");
    cflag::parse(arguments);

    EXPECT_TRUE(result);

    std::vector<std::string>& args = cflag::args();
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
    cflag::var(&result, "test", false, "test args.");
    arguments.clear();
    arguments.push_back("test-args");
    arguments.push_back("--test=true");
    arguments.push_back("arg00");
    arguments.push_back("arg01");
    cflag::parse(arguments);

    EXPECT_TRUE(result);

    std::vector<std::string>& args = cflag::args();
    EXPECT_EQ(2, args.size());
    EXPECT_STREQ(arg00.c_str(), args.at(0).c_str());
    EXPECT_STREQ(arg01.c_str(), args.at(1).c_str());
}

TEST(test_common, test_null_args) {
    bool result = false;
    std::vector<std::string> arguments;

    cflag::reset();
    cflag::var(&result, "test", false, "test args.");
    arguments.clear();
    arguments.push_back("test-args");
    arguments.push_back("--test=true");
    arguments.push_back("--");
    cflag::parse(arguments);

    EXPECT_TRUE(result);

    std::vector<std::string>& args = cflag::args();
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
    cflag::varp(&result, "test", "t", 0, "test missing value.");

    EXPECT_EXIT(cflag::parse(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*set flag.*value.*");
}

TEST(test_common, test_long_and_short_names_have_separate_lookups) {
    bool long_result = false;
    bool short_result = false;
    std::vector<std::string> arguments{"test-args", "--t", "-t"};

    cflag::reset();
    cflag::var(&long_result, "t", false, "long flag.");
    cflag::varp(&short_result, "test", "t", false, "short flag.");
    cflag::parse(arguments);

    EXPECT_TRUE(long_result);
    EXPECT_TRUE(short_result);
}

TEST(test_common, test_rejects_multi_character_short_name) {
    bool result = false;

    cflag::reset();

    EXPECT_EXIT(cflag::varp(&result, "test", "tt", false, "invalid short flag."), testing::ExitedWithCode(EXIT_FAILURE),
                ".*one character.*");
}
