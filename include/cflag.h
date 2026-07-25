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

#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace cflag {
    class c_flag_set;
    class c_flag;
    class i_value;

    // const and global variable
    extern const std::string k_bool_type_name;
    extern const std::string k_int_type_name;
    extern const std::string k_float_type_name;
    extern const std::string k_string_type_name;
    extern std::shared_ptr<c_flag_set> g_flag_set;
    extern const std::string k_help_flag_name;
    extern const std::string k_help_short_flag_name;
    extern const std::string k_global_flag_set_name;

    // bool value
    void bool_var(bool *arg, const char *name, bool default_value, const char *usage);
    void bool_var(bool *arg, std::string &name, bool default_value, std::string &usage);
    void bool_varp(bool *arg, const char *name, const char *short_name, bool default_value, const char *usage);
    void bool_varp(bool *arg, std::string &name, std::string &short_name, bool default_value, std::string &usage);

    // int value
    void int_var(int *arg, const char *name, int default_value, const char *usage);
    void int_var(int *arg, std::string &name, int default_value, std::string &usage);
    void int_varp(int *arg, const char *name, const char *short_name, int default_value, const char *usage);
    void int_varp(int *arg, std::string &name, std::string &short_name, int default_value, std::string &usage);

    // float value
    void float_var(float *arg, const char *name, float default_value, const char *usage);
    void float_var(float *arg, std::string &name, float default_value, std::string &usage);
    void float_varp(float *arg, const char *name, const char *short_name, float default_value, const char *usage);
    void float_varp(float *arg, std::string &name, std::string &short_name, float default_value, std::string &usage);

    // string value
    void string_var(std::string *arg, const char *name, const char *default_value, const char *usage);
    void string_var(std::string *arg, std::string &name, std::string &default_value, std::string &usage);
    void string_varp(std::string *arg, const char *name, const char *short_name, const char *default_value,
            const char *usage);
    void string_varp(std::string *arg, std::string &name, std::string &short_name, std::string &default_value,
            std::string &usage);

    void parse(int argc, char *argv[]);
    void parse(std::vector<std::string> &arguments);
    void reset();
    void usage();
    std::vector<std::string> &args();

    class c_flag_set {
    public:
        explicit c_flag_set(const std::string &name) : name_(name) {}
        c_flag_set(const c_flag_set &other) = delete;
        c_flag_set(c_flag_set &&other) = delete;
        c_flag_set &operator=(const c_flag_set &other) = delete;
        c_flag_set &operator=(c_flag_set &&other) = delete;
        ~c_flag_set() = default;

        void usage();
        void print_flags();
        void parse(int argc, char *argv[]);
        void parse(std::vector<std::string> &arguments);
        void reset();

        // bool value
        void bool_var(bool *arg, const char *name, bool default_value, const char *usage);
        void bool_var(bool *arg, std::string &name, bool default_value, std::string &usage);
        void bool_varp(bool *arg, const char *name, const char *short_name, bool default_value, const char *usage);
        void bool_varp(bool *arg, std::string &name, std::string &short_name, bool default_value, std::string &usage);

        // int value
        void int_var(int *arg, const char *name, int default_value, const char *usage);
        void int_var(int *arg, std::string &name, int default_value, std::string &usage);
        void int_varp(int *arg, const char *name, const char *short_name, int default_value, const char *usage);
        void int_varp(int *arg, std::string &name, std::string &short_name, int default_value, std::string &usage);

        // float value
        void float_var(float *arg, const char *name, float default_value, const char *usage);
        void float_var(float *arg, std::string &name, float default_value, std::string &usage);
        void float_varp(float *arg, const char *name, const char *short_name, float default_value, const char *usage);
        void float_varp(float *arg, std::string &name, std::string &short_name, float default_value, std::string &usage);

        // string value
        void string_var(std::string *arg, const char *name, const char *default_value, const char *usage);
        void string_var(std::string *arg, std::string &name, std::string &default_value, std::string &usage);
        void string_varp(std::string *arg, const char *name, const char *short_name, const char *default_value,
                const char *usage);
        void string_varp(std::string *arg, std::string &name, std::string &short_name, std::string &default_value,
                std::string &usage);

    private:
        std::shared_ptr<c_flag> lookup_(const std::string &name, bool short_name) const;
        void add_flag_(const std::shared_ptr<c_flag> &flag);
        void parse_long_args_(const std::string &seg, std::size_t &index,
                const std::vector<std::string> &arguments);
        void parse_short_args_(const std::string &seg, std::size_t &index,
                const std::vector<std::string> &arguments);

    public:
        // setter
        void program(const std::string &value) { program_ = value; }
        // getter
        const std::string &program() const { return program_; }
        std::vector<std::string> &args() { return args_; }

    private:
        std::string name_;
        std::string program_;
        std::map<std::string, std::shared_ptr<c_flag>> flags_;
        std::map<std::string, std::shared_ptr<c_flag>> short_flags_;
        std::vector<std::string> args_;
    }; // class c_flag_set


    class c_flag {
    public:
        explicit c_flag(const std::string &name) : name_(name) {}
        ~c_flag() = default;

    public:
        // setter
        void short_name(const std::string &short_name) { short_name_ = short_name; }
        void usage(const std::string &usage) { usage_ = usage; }
        void default_value(const std::string &default_value) { default_value_ = default_value; }
        void value(const std::shared_ptr<i_value> &value) { value_ = value; }
        // getter
        const std::string &name() const { return name_; }
        const std::string &usage() const { return usage_; }
        const std::string &short_name() const { return short_name_; }
        const std::string &default_value() const { return default_value_; }
        const std::shared_ptr<i_value> &value() const { return value_; }

    private:
        std::string name_;
        std::string short_name_;
        std::string usage_;
        std::string default_value_;
        std::shared_ptr<i_value> value_;
    }; // class c_flag


    class i_value {
    public:
        virtual ~i_value() = default;
        virtual bool set(std::string &value) = 0;
        virtual const std::string &type() = 0;
    }; // class i_value

} // namespace cflag
