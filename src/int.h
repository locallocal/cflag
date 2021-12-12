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

    const std::string k_int_type_name = "int";


    class c_int_value: public i_value {
    public:
        c_int_value(int *arg) { arg_ = arg; }
        ~c_int_value() = default;
        bool set(std::string &value) override;
        const std::string &type() override;

    private:
        int *arg_;
    }; // class c_int_value

} // namespace
