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

enum class test_mode {
    safe,
    fast,
};

namespace cflag {

template <>
struct flag_traits<test_mode> {
    static const std::string &type_name() {
        static const std::string value = "mode";
        return value;
    }

    static std::string format(test_mode value) {
        return value == test_mode::safe ? "safe" : "fast";
    }

    static bool parse(const std::string &value, test_mode &output) {
        if (value == "safe") {
            output = test_mode::safe;
            return true;
        }
        if (value == "fast") {
            output = test_mode::fast;
            return true;
        }
        return false;
    }

    static bool has_implicit_value() {
        return false;
    }
};

} // namespace cflag

TEST(test_template, test_generic_var_and_varp) {
    int port = 0;
    std::string config;
    const std::vector<std::string> arguments{
            "test-template", "--port", "8080", "-csettings.json"};

    cflag::reset();
    cflag::var(&port, "port", 80, "server port.");
    cflag::varp(&config, "config", "c", std::string("default.json"), "config file.");
    cflag::parse(arguments);

    EXPECT_EQ(8080, port);
    EXPECT_EQ("settings.json", config);
}

TEST(test_template, test_custom_flag_traits) {
    test_mode mode = test_mode::safe;
    const std::vector<std::string> arguments{"test-template", "--mode=fast"};

    cflag::reset();
    cflag::var(&mode, "mode", test_mode::safe, "execution mode.");
    cflag::parse(arguments);

    EXPECT_EQ(test_mode::fast, mode);
}
