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

int main(int argc, char *argv[]) {
    int port = 0;
    bool daemon = false;
    bool version = false;
    float point = 0.0;
    std::string ip;
    std::string conf_file;

    cflag::bool_varp(&daemon, "daemon", "d", false, "run with daemonize.");
    cflag::bool_varp(&version, "version", "v", false, "show server version.");
    cflag::int_varp(&port, "port", "p", 9999, "server tcp port.");
    cflag::float_varp(&point, "point", "k", 0.0, "percent of usage.");
    cflag::string_varp(&conf_file, "config", "c", "./config.conf", "config file of example.");
    cflag::string_var(&ip, "ip", "0.0.0.0", "server ip address.");
    cflag::parse(argc, argv);

    std::cout << "daemon: " << std::boolalpha << daemon << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "point: " << point << std::endl;
    std::cout << "conf_file: " << conf_file << std::endl;
    return 0;
}

