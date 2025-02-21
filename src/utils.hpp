//
// Created by taxmachine on 17/02/25.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <filesystem>
#include <vector>

namespace utils {
    std::vector<std::filesystem::directory_entry> getSONiXKeyboard();
    std::pair<uint16_t, uint16_t> getKeyboardIdentifiers(const std::filesystem::directory_entry& keyboard);
    std::vector<std::vector<uint8_t>> split_data(const uint8_t* data, size_t data_length);
}

#endif //UTILS_HPP
