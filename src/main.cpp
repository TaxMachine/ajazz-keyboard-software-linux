#include <csignal>
#include <string>
#include <iostream>

#include "GUI/app.hpp"
#include "utils.hpp"

auto keyboard = std::make_unique<AK820Pro>(AJAZZ_VID, AK820PRO_PID);

void signalHandler(int signum) {
    keyboard.reset();
    exit(0);
}

int main(int argc, char* argv[]) {
    signal(SIGABRT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);
    try {
        AjazzGUI gui("Ajazz Keyboard software", *keyboard);
        gui.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}