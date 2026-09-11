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

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "cflag.h"

namespace {

class test_flag_file : public testing::Test {
protected:
    void SetUp() override {
        flags_.varp(&daemon_, "daemon", "d", false, "run as a daemon.");
        flags_.varp(&port_, "port", "p", 9999, "server port.");
        flags_.var(&ratio_, "ratio", 0.25, "ratio.");
        flags_.var(&name_, "name", std::string("default"), "name.");
    }

    void TearDown() override {
        for (std::size_t index = 0; index < files_.size(); ++index) {
            std::remove(files_[index].c_str());
        }
    }

    std::string write_file(const std::string& file_name, const std::string& content) {
        const std::string path = testing::TempDir() + "cflag_" + std::to_string(counter_++) + "_" + file_name;
        std::ofstream output(path.c_str(), std::ios::out | std::ios::binary);
        output << content;
        output.close();
        files_.push_back(path);
        return path;
    }

    void parse(const std::vector<std::string>& options) {
        std::vector<std::string> arguments;
        arguments.push_back("test-flag-file");
        arguments.insert(arguments.end(), options.begin(), options.end());
        flags_.parse(arguments);
    }

    cflag::flag_set flags_;
    bool daemon_ = false;
    int port_ = 0;
    double ratio_ = 0.0;
    std::string name_;

private:
    static int counter_;
    std::vector<std::string> files_;
};

int test_flag_file::counter_ = 0;

TEST_F(test_flag_file, gflags_format) {
    const std::string path = write_file("flags.txt",
                                        "# a comment\n"
                                        "\n"
                                        "--port=8080\n"
                                        "  --daemon  \n"
                                        "--name=with space\n"
                                        "\r\n");

    parse({"--flag-file=" + path});

    EXPECT_TRUE(daemon_);
    EXPECT_EQ(8080, port_);
    EXPECT_EQ("with space", name_);
}

TEST_F(test_flag_file, gflags_format_short_flags) {
    const std::string path = write_file("flags.txt", "-d\n-p8080\n");

    parse({"--flag-file", path});

    EXPECT_TRUE(daemon_);
    EXPECT_EQ(8080, port_);
}

TEST_F(test_flag_file, json_format) {
    const std::string path = write_file("flags.json",
                                        "{\n"
                                        "  \"port\": 8080,\n"
                                        "  \"daemon\": true,\n"
                                        "  \"ratio\": -1.5e1,\n"
                                        "  \"name\": \"tab\\tquote\\\" \\u4e2d\\ud83d\\ude00\"\n"
                                        "}\n");

    parse({"--flag-file=" + path});

    EXPECT_TRUE(daemon_);
    EXPECT_EQ(8080, port_);
    EXPECT_DOUBLE_EQ(-15.0, ratio_);
    EXPECT_EQ("tab\tquote\" \xe4\xb8\xad\xf0\x9f\x98\x80", name_);
}

TEST_F(test_flag_file, json_empty_object) {
    const std::string path = write_file("flags.json", " {} ");

    parse({"--flag-file=" + path});

    EXPECT_EQ(9999, port_);
}

TEST_F(test_flag_file, yaml_format) {
    const std::string path = write_file("flags.yaml",
                                        "---\n"
                                        "# comment\n"
                                        "port: 8080   # trailing comment\n"
                                        "daemon: true\n"
                                        "ratio: 0.5\n"
                                        "\"name\": 'it''s # not a comment'\n"
                                        "...\n");

    parse({"--flag-file=" + path});

    EXPECT_TRUE(daemon_);
    EXPECT_EQ(8080, port_);
    EXPECT_DOUBLE_EQ(0.5, ratio_);
    EXPECT_EQ("it's # not a comment", name_);
}

TEST_F(test_flag_file, yaml_double_quoted_and_empty_values) {
    const std::string path = write_file("flags.yml",
                                        "name: \"line\\nbreak \\u00e9\"\n"
                                        "port: 1\n");
    parse({"--flag-file=" + path});
    EXPECT_EQ("line\nbreak \xc3\xa9", name_);

    const std::string empty = write_file("flags.yml", "name:\n");
    parse({"--flag-file=" + empty});
    EXPECT_EQ("", name_);
}

TEST_F(test_flag_file, detects_format_from_content) {
    const std::string json = write_file("noext", "  {\"port\": 1}");
    const std::string gflags = write_file("noext", "# c\n--port=2\n");
    const std::string yaml = write_file("noext", "# c\nport: 3\n");

    parse({"--flag-file=" + json});
    EXPECT_EQ(1, port_);
    parse({"--flag-file=" + gflags});
    EXPECT_EQ(2, port_);
    parse({"--flag-file=" + yaml});
    EXPECT_EQ(3, port_);
}

TEST_F(test_flag_file, explicit_format) {
    const std::string path = write_file("flags.json", "port: 4\n");

    flags_.parse_file(path, cflag::flag_file_format::yaml);

    EXPECT_EQ(4, port_);
}

TEST_F(test_flag_file, later_arguments_override_file) {
    const std::string path = write_file("flags.json", "{\"port\": 1, \"daemon\": true}");

    parse({"--port=5", "--flag-file=" + path, "--port=6", "positional"});

    EXPECT_EQ(6, port_);
    EXPECT_TRUE(daemon_);
    ASSERT_EQ(1u, flags_.args().size());
    EXPECT_EQ("positional", flags_.args()[0]);
}

TEST_F(test_flag_file, nested_flag_files) {
    const std::string inner = write_file("inner.yaml", "ratio: 2\n");
    const std::string middle = write_file("middle.json", "{\"flag-file\": \"" + inner + "\", \"port\": 7}");
    const std::string outer = write_file("outer.txt", "--flag-file=" + middle + "\n--daemon\n");

    parse({"--flag-file=" + outer});

    EXPECT_TRUE(daemon_);
    EXPECT_EQ(7, port_);
    EXPECT_DOUBLE_EQ(2.0, ratio_);
}

TEST_F(test_flag_file, global_parse_file) {
    int port = 0;
    cflag::reset();
    cflag::var(&port, "port", 0, "port.");
    const std::string path = write_file("flags.json", "{\"port\": 8}");

    cflag::parse_file(path);

    EXPECT_EQ(8, port);
    cflag::reset();
}

TEST_F(test_flag_file, missing_value_fails) {
    EXPECT_EXIT(parse({"--flag-file"}), testing::ExitedWithCode(EXIT_FAILURE), "please set flag flag-file value");
}

TEST_F(test_flag_file, missing_file_fails) {
    EXPECT_EXIT(parse({"--flag-file=" + testing::TempDir() + "cflag_missing_file"}),
                testing::ExitedWithCode(EXIT_FAILURE), "cannot open flag file");
}

TEST_F(test_flag_file, unknown_flag_reports_file) {
    const std::string path = write_file("flags.json", "{\"nope\": 1}");
    EXPECT_EXIT(parse({"--flag-file=" + path}), testing::ExitedWithCode(EXIT_FAILURE),
                "flag file .*flags.json: flag nope not exist");
}

TEST_F(test_flag_file, invalid_value_reports_file) {
    const std::string path = write_file("flags.yaml", "port: abc\n");
    EXPECT_EXIT(parse({"--flag-file=" + path}), testing::ExitedWithCode(EXIT_FAILURE),
                "flag file .*flags.yaml: invalid value for port");
}

TEST_F(test_flag_file, positional_arguments_in_file_fail) {
    const std::string path = write_file("flags.txt", "--port=1\npositional\n");
    EXPECT_EXIT(parse({"--flag-file=" + path}), testing::ExitedWithCode(EXIT_FAILURE),
                "flags.txt:2: expected a flag starting with '-'");
}

TEST_F(test_flag_file, self_reference_fails) {
    const std::string path = testing::TempDir() + "cflag_self.txt";
    write_file("self.txt", "");
    std::ofstream output(path.c_str());
    output << "--flag-file=" << path << "\n";
    output.close();
    EXPECT_EXIT(parse({"--flag-file=" + path}), testing::ExitedWithCode(EXIT_FAILURE), "nested too deeply");
    std::remove(path.c_str());
}

TEST_F(test_flag_file, json_errors) {
    const std::string nested = write_file("flags.json", "{\"port\": {\"x\": 1}}");
    EXPECT_EXIT(parse({"--flag-file=" + nested}), testing::ExitedWithCode(EXIT_FAILURE),
                "flags.json:1: nested objects and arrays are not supported");

    const std::string null_value = write_file("flags.json", "{\n\"port\": null}");
    EXPECT_EXIT(parse({"--flag-file=" + null_value}), testing::ExitedWithCode(EXIT_FAILURE),
                "flags.json:2: null values are not supported");

    const std::string trailing = write_file("flags.json", "{\"port\": 1,}");
    EXPECT_EXIT(parse({"--flag-file=" + trailing}), testing::ExitedWithCode(EXIT_FAILURE),
                "expected a quoted flag name");

    const std::string garbage = write_file("flags.json", "{\"port\": 1} x");
    EXPECT_EXIT(parse({"--flag-file=" + garbage}), testing::ExitedWithCode(EXIT_FAILURE),
                "unexpected content after the closing");

    const std::string unterminated = write_file("flags.json", "{\"port\": \"abc");
    EXPECT_EXIT(parse({"--flag-file=" + unterminated}), testing::ExitedWithCode(EXIT_FAILURE), "unterminated string");
}

TEST_F(test_flag_file, yaml_errors) {
    const std::string nested = write_file("flags.yaml", "port:\n  x: 1\n");
    EXPECT_EXIT(parse({"--flag-file=" + nested}), testing::ExitedWithCode(EXIT_FAILURE),
                "flags.yaml:2: nested mappings are not supported");

    const std::string list = write_file("flags.yaml", "- port\n");
    EXPECT_EXIT(parse({"--flag-file=" + list}), testing::ExitedWithCode(EXIT_FAILURE), "lists are not supported");

    const std::string flow = write_file("flags.yaml", "port: [1, 2]\n");
    EXPECT_EXIT(parse({"--flag-file=" + flow}), testing::ExitedWithCode(EXIT_FAILURE),
                "flow collections are not supported");

    const std::string no_colon = write_file("flags.yaml", "port=1\n");
    EXPECT_EXIT(parse({"--flag-file=" + no_colon}), testing::ExitedWithCode(EXIT_FAILURE), "expected 'name: value'");

    const std::string trailing = write_file("flags.yaml", "name: \"a\" b\n");
    EXPECT_EXIT(parse({"--flag-file=" + trailing}), testing::ExitedWithCode(EXIT_FAILURE),
                "unexpected content after the quoted value");
}

TEST_F(test_flag_file, reserved_name) {
    int value = 0;
    EXPECT_EXIT(flags_.var(&value, "flag-file", 0, "reserved."), testing::ExitedWithCode(EXIT_FAILURE),
                "reserved for flag-file");
}

}  // namespace
