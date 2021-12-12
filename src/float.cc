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

#include "float.h"

bool cflag::c_float_value::set(std::string &value) {
    try {
        *arg_ = std::stof(value);
        return true;
    } catch(...) {
        return false;
    }
}

const std::string &cflag::c_float_value::type() {
    return k_float_type_name;
}

void cflag::float_var(float *arg, const char *name, float default_value, const char *usage) {
    g_flag_set->float_var(arg, name, default_value, usage);
}

void cflag::float_var(float *arg, std::string &name, float default_value, std::string &usage) {
    g_flag_set->float_var(arg, name, default_value, usage);
}

void cflag::float_varp(float *arg, const char *name, const char *short_name, float default_value, const char *usage) {
    g_flag_set->float_varp(arg, name, short_name, default_value, usage);
}

void cflag::float_varp(float *arg, std::string &name, std::string &short_name, float default_value, std::string &usage) {
    g_flag_set->float_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::float_var(float *arg, const char *name, float default_value, const char *usage) {
    std::string flag_name = name;
    std::string flag_usage = usage;
    float_var(arg, flag_name, default_value, flag_usage);
}

void cflag::c_flag_set::float_var(float *arg, std::string &name, float default_value, std::string &usage) {
    std::string short_name;
    float_varp(arg, name, short_name, default_value, usage);
}

void cflag::c_flag_set::float_varp(float *arg, const char *name, const char *short_name, float default_value,
        const char *usage) {
    std::string flag_name = name;
    std::string flag_short_name = short_name;
    std::string flag_usage = usage;
    float_varp(arg, flag_name, flag_short_name, default_value, flag_usage);
}

void cflag::c_flag_set::float_varp(float *arg, std::string &name, std::string &short_name, float default_value,
        std::string &usage) {
    std::shared_ptr<c_flag> flag = std::make_shared<c_flag>(name);
    flag->short_name(short_name);
    flag->usage(usage);
    std::string default_value_str = std::to_string(default_value);
    flag->default_value(default_value_str);

    std::shared_ptr<i_value> float_value = std::make_shared<c_float_value>(arg);
    float_value->set(default_value_str);
    flag->value(float_value);
    add_flag_(flag);
}
