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

#include <cstdlib>
#include <iostream>

const std::string cflag::k_help_flag_name = "help";
const std::string cflag::k_help_short_flag_name = "h";
const std::string cflag::k_global_flag_set_name = "global";

std::shared_ptr<cflag::c_flag_set> cflag::g_flag_set =
        std::make_shared<cflag::c_flag_set>(cflag::k_global_flag_set_name);

void cflag::parse(int argc, char *argv[]) {
    g_flag_set->parse(argc, argv);
}

void cflag::parse(std::vector<std::string> &arguments) {
    g_flag_set->parse(arguments);
}

void cflag::usage() {
    g_flag_set->usage();
}

void cflag::reset() {
    g_flag_set->reset();
}

std::vector<std::string> &cflag::args() {
    return g_flag_set->args();
}

void cflag::c_flag_set::usage() {
    std::cout << "Usage: " << program() << " [options]\n\n";
    print_flags();
}

void cflag::c_flag_set::print_flags() {
    for (const auto &entry : flags_) {
        const auto &flag = entry.second;
        if (!flag->short_name().empty()) {
            std::cout << " -" << flag->short_name() << "  ";
        } else {
            std::cout << "     ";
        }
        if (!flag->name().empty()) {
            std::cout << "--" << flag->name();
        } else {
            std::cout << "    ";
        }
        std::cout << "[" << flag->value()->type() << "]\t";
        std::cout << flag->usage();
        if (!flag->default_value().empty()) {
            std::cout << "(" << flag->default_value() << ")";
        }
        std::cout << '\n';
    }
}

void cflag::c_flag_set::parse(int argc, char *argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(argc > 0 ? static_cast<std::size_t>(argc) : 0);

    for (int index = 0; index < argc; ++index) {
        arguments.push_back(argv[index]);
    }
    parse(arguments);
}

void cflag::c_flag_set::parse(std::vector<std::string> &arguments) {
    args_.clear();
    if (arguments.empty()) {
        program_.clear();
        return;
    }

    program(arguments.front());

    for (std::size_t index = 1; index < arguments.size(); ++index) {
        const std::string &seg = arguments[index];
        if (seg == "--") {
            args_.insert(args_.cend(), arguments.begin() + index + 1, arguments.end());
            break;
        }
        if (seg.compare(0, 3, "---") == 0) {
            std::cerr << "invalid argument " << seg << '\n';
            exit(EXIT_FAILURE);
        }

        if (seg.size() > 2 && seg.compare(0, 2, "--") == 0) {
            parse_long_args_(seg, index, arguments);
            continue;
        }
        if (seg.size() > 1 && seg.front() == '-') {
            parse_short_args_(seg, index, arguments);
            continue;
        }
        args_.push_back(seg);
    }
}

void cflag::c_flag_set::parse_long_args_(const std::string &seg, std::size_t &index,
        const std::vector<std::string> &arguments) {
    std::string flag_name;
    std::string flag_value;
    std::string arg = seg.substr(2);

    // get arg name and value
    const std::size_t found = arg.find('=');
    const bool has_inline_value = found != std::string::npos;
    if (has_inline_value) {
        flag_name = arg.substr(0, found);
        flag_value = arg.substr(found + 1);
    } else {
        flag_name = arg;
    }

    // help
    if (flag_name == k_help_flag_name) {
        usage();
        exit(EXIT_SUCCESS);
    }

    // set flag value
    auto flag = lookup_(flag_name, false);
    if (flag == nullptr) {
        std::cerr << "flag " << flag_name << " not exist.\n";
        exit(EXIT_FAILURE);
    }
    const auto &value = flag->value();
    if (value->type() == k_bool_type_name && !has_inline_value) {
        flag_value = "true";
    } else if (!has_inline_value) {
        if (index + 1 >= arguments.size()) {
            std::cerr << "please set flag " << flag_name << " value.\n";
            exit(EXIT_FAILURE);
        }
        flag_value = arguments[++index];
    }
    if (!value->set(flag_value)) {
        std::cerr << "invalid value for " << flag_name << ".\n";
        exit(EXIT_FAILURE);
    }
}

void cflag::c_flag_set::parse_short_args_(const std::string &seg, std::size_t &index,
        const std::vector<std::string> &arguments) {
    std::string flag_name;
    std::string flag_value;
    std::string arg = seg.substr(1);

    for (std::size_t arg_index = 0; arg_index < arg.size(); ++arg_index) {
        flag_name.clear();
        flag_name.push_back(arg.at(arg_index));
        if (flag_name == k_help_short_flag_name) {
            usage();
            exit(EXIT_SUCCESS);
        }

        auto flag = lookup_(flag_name, true);
        if (flag == nullptr) {
            std::cerr << "flag " << flag_name << " not exist.\n";
            exit(EXIT_FAILURE);
        }
        const auto &value = flag->value();
        if (value->type() == cflag::k_bool_type_name) {
            flag_value = "true";
        } else {
            if (arg_index == arg.size() - 1) {
                if (index + 1 >= arguments.size()) {
                    std::cerr << "please set flag " << flag_name << " value.\n";
                    exit(EXIT_FAILURE);
                }
                flag_value = arguments[++index];
            } else {
                flag_value = arg.substr(arg_index + 1);
                arg_index = arg.size() - 1;
            }
        }
        if (!value->set(flag_value)) {
            std::cerr << "invalid value for " << flag_name << ".\n";
            exit(EXIT_FAILURE);
        }
    }
}

std::shared_ptr<cflag::c_flag> cflag::c_flag_set::lookup_(const std::string &name, bool short_name) const {
    const auto &flags = short_name ? short_flags_ : flags_;
    const auto it = flags.find(name);
    if (it != flags.end()) {
        return it->second;
    }
    return nullptr;
}

void cflag::c_flag_set::add_flag_(const std::shared_ptr<cflag::c_flag> &flag) {
    const std::string &name = flag->name();
    const std::string &short_name = flag->short_name();

    if (name.empty() && short_name.empty()) {
        std::cerr << "flag name cannot be empty.\n";
        exit(EXIT_FAILURE);
    }
    if (name == k_help_flag_name || short_name == k_help_short_flag_name) {
        std::cerr << "flag name is reserved for help.\n";
        exit(EXIT_FAILURE);
    }
    if (!short_name.empty() && short_name.size() != 1) {
        std::cerr << "short flag name must contain one character.\n";
        exit(EXIT_FAILURE);
    }

    if (!name.empty()) {
        auto it = flags_.find(name);
        if (it != flags_.end()) {
            std::cerr << "redefine flag " << name << ".\n";
            exit(EXIT_FAILURE);
        }
        flags_[name] = flag;
    }

    if (!short_name.empty()) {
        auto it = short_flags_.find(short_name);
        if (it != short_flags_.end()) {
            std::cerr << "redefine flag " << short_name << ".\n";
            exit(EXIT_FAILURE);
        }
        short_flags_[short_name] = flag;
    }
}

void cflag::c_flag_set::reset() {
    flags_.clear();
    short_flags_.clear();
    args_.clear();
    program_.clear();
}
