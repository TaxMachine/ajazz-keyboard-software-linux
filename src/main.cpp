#include <string>
#include <vector>
#include <iostream>

#include "Ajazz.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {
    try {
        const Ajazz keyboard(AJAZZ_VID, AK820PRO_PID);

        auto [vendorID, productID, manufacturer,
            product, serialNumber] = keyboard.getHardwareInformation();
        std::cout << "Keyboard Information" << std::endl;
        std::cout << "Vendor ID: 0x" << std::hex << vendorID << std::dec << std::endl;
        std::cout << "Product ID: 0x" << std::hex << productID << std::dec << std::endl;
        std::wcout << "Manufacturer: " << manufacturer << std::endl;
        std::wcout << "Product: " << product << std::endl;
        std::wcout << "Serial Number: " << serialNumber << std::endl;

        std::cout << "changing mode to explode" << std::endl;
        keyboard.setMode(LightingMode::GLITTERING, 210, 66, 69, false);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}