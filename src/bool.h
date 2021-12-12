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

#include "cflag.h"

namespace cflag {
    
    const std::string k_bool_type_name = "bool";


    class c_bool_value: public i_value {
        const std::vector<std::string> true_values {"TRUE", "True", "T", "true", "t", "1"};
        const std::vector<std::string> false_values {"FALSE", "False", "F", "false", "f", "0"};
        
    public:
        c_bool_value(bool *arg) { arg_ = arg; }
        ~c_bool_value() = default;
        bool set(std::string &value) override;
        const std::string &type() override;

    private:
        bool *arg_;
    }; // class c_bool_value

} // namespace

