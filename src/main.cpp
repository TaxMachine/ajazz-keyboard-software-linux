#include <string>
#include <vector>
#include <iostream>

#include "Ajazz.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {
    try {
        const auto& keyboards = utils::getSONiXKeyboard();
        std::cout << keyboards[0] << std::endl;
        const auto&[fst, snd] = utils::getKeyboardIdentifiers(keyboards[0]);
        std::cout << "VID: " << fst << " " << "PID: " << snd << std::endl;

        const Ajazz keyboard(AJAZZ_VID, AK820PRO_PID);

        std::cout << "changing mode to ripples" << std::endl;
        keyboard.setMode(RIPPLES, 210, 66, 69, false);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
