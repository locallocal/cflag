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

std::shared_ptr<cflag::c_flag_set> cflag::g_flag_set =
        std::make_shared<cflag::c_flag_set>(const_cast<std::string &>(cflag::k_global_flag_set_name));

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
    std::cout << "Usage: " << program() << " [options]" << std::endl;
    std::cout << std::endl;
    print_flags();
}

void cflag::c_flag_set::print_flags() {
    for (auto it = flags_.cbegin(); it != flags_.cend(); it++) {
        auto flag = it->second;
        if (!flag->short_name().empty()) {
            std::cout << " -" << it->second->short_name() << "  ";
        } else {
            std::cout << "     "; 
        }
        if (!flag->name().empty()) {
            std::cout << "--" << it->second->name();
        } else {
            std::cout << "    ";
        }
        std::cout << "[" << flag->value()->type() << "]\t";
        std::cout << flag->usage();
        if (!flag->default_value().empty()) {
            std::cout << "(" << flag->default_value() << ")" << std::endl;
        }
    }
}

void cflag::c_flag_set::parse(int argc, char *argv[]) {
    int index = 0;
    std::vector<std::string> arguments;

    for (index = 0; index < argc; index++) {
        arguments.push_back(argv[index]);
    }
    parse(arguments);
}

void cflag::c_flag_set::parse(std::vector<std::string> &arguments) {
    int index = 0;
    program(arguments[0]);

    for (index = 1; index < arguments.size(); index++) {
        std::string seg = arguments.at(index);
        if (seg.size() == 2 && seg == "--") {
            args_.insert(args_.cend(), arguments.begin() + index + 1, arguments.end());
            break;
        }
        if (seg.size() > 2 && seg.substr(0, 3) == "---") {
            std::cout << "invalid argument " << seg << std::endl;
            exit(EXIT_FAILURE);
        }

        if (seg.at(0) == '-' && seg.at(1) == '-') {
            parse_long_args_(seg, index, arguments);
            continue;
        } else if (seg.at(0) == '-') {
            parse_short_args_(seg, index, arguments);
            continue;
        }
        args_.push_back(arguments.at(index));
    }
}

void cflag::c_flag_set::parse_long_args_(std::string &seg, int &index, std::vector<std::string> &arguments) {
    std::string flag_name;
    std::string flag_value;
    std::string arg = seg.substr(2);

    // get arg name and value
    std::size_t found = arg.find("=");
    if (found != std::string::npos) {
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
    auto flag = lookup_(flag_name);
    if (flag == nullptr) {
        std::cerr << "flag " << flag_name << " not exist." << std::endl;
        exit(EXIT_FAILURE);
    }
    auto value = flag->value();
    if (value->type() != k_bool_type_name && flag_value.empty()) {
        std::cerr << "please set flag " << flag_name << " value." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (value->type() == k_bool_type_name && flag_value.empty()) {
        flag_value = "true";
    }
    if (!value->set(flag_value)) {
        std::cerr << "invalid value for " << flag_name << "." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void cflag::c_flag_set::parse_short_args_(std::string &seg, int &index, std::vector<std::string> &arguments) {
    std::string flag_name;
    std::string flag_value;
    std::string arg = seg.substr(1);
    int arg_index = 0;

    for(arg_index = 0; arg_index < arg.size(); arg_index++) {
        flag_name.clear();
        flag_name.push_back(arg.at(arg_index));
        if (flag_name == k_help_short_flag_name) {
            usage();
            exit(EXIT_SUCCESS);
        }

        auto flag = lookup_(flag_name);
        if (flag == nullptr) {
            std::cerr << "flag " << flag_name << " not exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        auto value = flag->value();
        if (value->type() == cflag::k_bool_type_name) {
            flag_value = "true";
        } else {
            if (arg_index == arg.size() - 1) {
                index++;
                flag_value = arguments.at(index);
            } else {
                arg_index++;
                flag_value = arg.substr(arg_index);
                arg_index = arg.size();
            }
        }
        if (!value->set(flag_value)) {
            std::cerr << "invalid value for " << flag_name << "." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

std::shared_ptr<cflag::c_flag> cflag::c_flag_set::lookup_(std::string &name) {
    auto it = flags_.find(name);
    if (it != flags_.end()) {
        return it->second;
    }
    auto s_it = short_flags_.find(name);
    if (s_it != short_flags_.end()) {
        return s_it->second;
    }
    return nullptr;
}

void cflag::c_flag_set::add_flag_(std::shared_ptr<cflag::c_flag> flag) {
    std::string name = flag->name();
    if (!name.empty()) {
        auto it = flags_.find(name);
        if (it != flags_.end()) {
            std::cerr << "redefine flag " << name << "." << std::endl;
            exit(EXIT_FAILURE);
        }
        flags_[name] = flag;
    }
        
    std::string short_name = flag->short_name();
    if (!short_name.empty()) {
        auto it = short_flags_.find(short_name);
        if (it != short_flags_.end()) {
            std::cerr << "redefine flag " << short_name << "." << std::endl;
            exit(EXIT_FAILURE);
        }
        short_flags_[short_name] = flag;
    }
}

void cflag::c_flag_set::reset() {
    flags_.clear();
    short_flags_.clear();
    args_.clear();
}
