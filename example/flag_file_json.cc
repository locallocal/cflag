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

// Reads server flags from a JSON file with cflag::parse_file.
//
// The file path may be given as the first positional argument; otherwise the
// bundled example/flags.json is used. Command-line flags are applied first,
// then the file, so values in the file override them.

#include <iostream>
#include <string>

#include "cflag.h"

#ifndef CFLAG_EXAMPLE_DIR
#define CFLAG_EXAMPLE_DIR "example"
#endif

int main(int argc, char* argv[]) {
    int port = 0;
    bool daemon = false;
    float point = 0.0f;
    std::string ip;
    std::string conf_file;

    cflag::varp(&daemon, "daemon", "d", false, "run with daemonize.");
    cflag::varp(&port, "port", "p", 9999, "server tcp port.");
    cflag::varp(&point, "point", "k", 0.0f, "percent of usage.");
    cflag::varp(&conf_file, "config", "c", "./config.conf", "config file of example.");
    cflag::var(&ip, "ip", "0.0.0.0", "server ip address.");
    cflag::parse(argc, argv);

    const std::string path = cflag::args().empty() ? CFLAG_EXAMPLE_DIR "/flags.json" : cflag::args()[0];
    cflag::parse_file(path, cflag::flag_file_format::json);

    std::cout << "flag file: " << path << std::endl;
    std::cout << "daemon: " << std::boolalpha << daemon << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "point: " << point << std::endl;
    std::cout << "ip: " << ip << std::endl;
    std::cout << "conf_file: " << conf_file << std::endl;
    return 0;
}
