//
// Created by taxmachine on 17/02/25.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <filesystem>
#include <vector>

namespace utils {
    std::vector<std::vector<uint8_t>> split_data(const uint8_t* data, size_t data_length);
    std::string wstr2str(const std::wstring& wstr);
    void printHexData(const unsigned char *data, int length);
    void MessageBox(const std::string& title, const std::string& message);
    std::string openFileDialog(const char* filters);
}

#endif //UTILS_HPP
