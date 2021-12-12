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

#include "int.h"

bool cflag::c_int_value::set(std::string &value) {
    try {
        *arg_ = std::stoi(value, nullptr);
        return true;
    } catch (...) {
        return false;
    }
}

const std::string &cflag::c_int_value::type() {
    return k_int_type_name;
}

void cflag::int_var(int *arg, const char *name, int default_value, const char* usage) {
    g_flag_set->int_var(arg, name, default_value, usage);
}

void cflag::int_var(int *arg, std::string &name, int default_value, std::string &usage) {
    g_flag_set->int_var(arg, name, default_value, usage);
}

void cflag::int_varp(int *arg, const char *name, const char *short_name, int default_value, const char *usage) {
    g_flag_set->int_varp(arg, name, short_name, default_value, usage);
}

void cflag::int_varp(int *arg, std::string &name, std::string &short_name, int default_value, std::string &usage) {
    g_flag_set->int_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::int_var(int *arg, const char *name, int default_value, const char *usage) {
    std::string flag_name = name;
    std::string flag_usage = usage;
    int_var(arg, flag_name, default_value, flag_usage);
}

void cflag::c_flag_set::int_var(int *arg, std::string &name, int default_value, std::string &usage) {
    std::string short_name{};
    return int_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::int_varp(int *arg, const char *name, const char *short_name, int default_value,
        const char *usage) {
    std::string flag_name = name;
    std::string flag_short_name = short_name;
    std::string flag_usage = usage;
    int_varp(arg, flag_name, flag_short_name, default_value, flag_usage);
}

void cflag::c_flag_set::int_varp(int *arg, std::string &name, std::string &short_name, int default_value,
        std::string &usage) {
    std::shared_ptr<c_flag> flag = std::make_shared<c_flag>(name);
    flag->short_name(short_name);
    flag->usage(usage);

    std::string default_value_str = std::to_string(default_value);
    flag->default_value(default_value_str);

    std::shared_ptr<i_value> int_value = std::make_shared<c_int_value>(arg);
    int_value->set(default_value_str);
    flag->value(int_value);
    add_flag_(flag);
}
