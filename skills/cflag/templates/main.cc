// Starter program using cflag. Replace the flags with the project's own.
#include <iostream>
#include <string>

#include "cflag.h"

int main(int argc, char* argv[]) {
    int port = 0;
    bool daemon = false;
    std::string config;

    // Register before parse. Defaults must match the target type exactly.
    cflag::varp(&port, "port", "p", 8080, "tcp port to listen on.");
    cflag::varp(&daemon, "daemon", "d", false, "run in the background.");
    cflag::var(&config, "config", std::string("./app.conf"), "path to the config file.");

    // Exits with EXIT_FAILURE on any error; --help/-h prints usage and exits 0.
    // --flag-file=<path> is available automatically.
    cflag::parse(argc, argv);

    std::cout << "port=" << port << " daemon=" << std::boolalpha << daemon << " config=" << config << '\n';
    for (const std::string& positional : cflag::args()) {
        std::cout << "positional: " << positional << '\n';
    }
    return 0;
}
