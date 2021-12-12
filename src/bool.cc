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

#include "bool.h"

bool cflag::c_bool_value::set(std::string &value) {
    for (auto it = true_values.cbegin(); it != true_values.cend(); it++) {
        if (value == *it) {
            *arg_ = true;
            return true;
        }
    }
    for (auto it = false_values.cbegin(); it != false_values.cend(); it++) {
        if (value == *it) {
            *arg_ = false;
            return true;
        }
    }
    return false;
}

const std::string &cflag::c_bool_value::type() {
    return k_bool_type_name;
}

void cflag::bool_var(bool *arg, const char* name, bool default_value, const char* usage) {
    g_flag_set->bool_var(arg, name, default_value, usage);
}

void cflag::bool_var(bool *arg, std::string &name, bool default_value, std::string &usage) {
    g_flag_set->bool_var(arg, name, default_value, usage);
}

void cflag::bool_varp(bool *arg, const char *name, const char *short_name, bool default_value, const char* usage) {
    g_flag_set->bool_varp(arg, name, short_name, default_value, usage);
}

void cflag::bool_varp(bool *arg, std::string &name, std::string &short_name, bool default_value, std::string &usage) {
    g_flag_set->bool_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::bool_var(bool *arg, const char *name, bool default_value, const char *usage) {
    std::string flag_name = name;
    std::string flag_usage = usage;
    bool_var(arg, flag_name, default_value, flag_usage);
}

void cflag::c_flag_set::bool_var(bool *arg, std::string &name, bool default_value, std::string &usage) {
    std::string short_name{};
    bool_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::bool_varp(bool *arg, const char *name, const char *short_name, bool default_value,
        const char *usage) {
    std::string flag_name = name;
    std::string flag_short_name = short_name;
    std::string flag_usage = usage;
    bool_varp(arg, flag_name, flag_short_name, default_value, flag_usage);
}

void cflag::c_flag_set::bool_varp(bool *arg, std::string &name, std::string &short_name, bool default_value,
        std::string &usage) {
    std::shared_ptr<c_flag> flag = std::make_shared<c_flag>(name);
    flag->usage(usage);
    flag->short_name(short_name);

    std::string default_value_str;
    if (default_value) {
        default_value_str = "true";
    } else {
        default_value_str = "false";
    }
    flag->default_value(default_value_str);

    std::shared_ptr<i_value> bool_value = std::make_shared<c_bool_value>(arg);
    bool_value->set(default_value_str);
    flag->value(bool_value);
    add_flag_(flag);
}
