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

#include "string.h"

bool cflag::c_string_value::set(std::string &value) {
    *arg_ = value;
    return true;
}

const std::string &cflag::c_string_value::type() {
    return k_string_type_name;
}

void cflag::string_var(std::string *arg, const char *name, const char *default_value, const char *usage) {
    g_flag_set->string_var(arg, name, default_value, usage);
}

void cflag::string_var(std::string *arg, std::string &name, std::string &default_value, std::string &usage) {
    g_flag_set->string_var(arg, name, default_value, usage);
}

void cflag::string_varp(std::string *arg, const char *name, const char *short_name, const char *default_value,
        const char *usage) {
    g_flag_set->string_varp(arg, name, short_name, default_value, usage);
}

void cflag::string_varp(std::string *arg, std::string &name, std::string &short_name, std::string &default_value,
        std::string &usage) {
    g_flag_set->string_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::string_var(std::string *arg, const char *name, const char *default_value, const char *usage) {
    std::string flag_name = name;
    std::string flag_default_value = default_value;
    std::string flag_usage = usage;
    string_var(arg, flag_name, flag_default_value, flag_usage);
}

void cflag::c_flag_set::string_var(std::string *arg, std::string &name, std::string &default_value, std::string &usage) {
    std::string short_name{};
    string_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::string_varp(std::string *arg, const char *name, const char *short_name,
        const char *default_value, const char *usage) {
    std::string flag_name = name;
    std::string flag_short_name = short_name;
    std::string flag_default_value = default_value;
    std::string flag_usage = usage;
    string_varp(arg, flag_name, flag_short_name, flag_default_value, flag_usage);
}

void cflag::c_flag_set::string_varp(std::string *arg, std::string &name, std::string &short_name,
        std::string &default_value, std::string &usage) {
    std::shared_ptr<c_flag> flag = std::make_shared<c_flag>(name);
    flag->short_name(short_name);
    flag->usage(usage);
    flag->default_value(default_value);

    std::shared_ptr<i_value> string_value = std::make_shared<c_string_value>(arg);
    string_value->set(default_value);
    flag->value(string_value);
    add_flag_(flag);
}

