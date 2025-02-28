//
// Created by taxmachine on 17/02/25.
//

#include "utils.hpp"

#include <cstring>
#include <regex>
#include <fstream>
#include <iostream>

#include "../keyboards/keyboarddefs.hpp"

std::vector<std::vector<uint8_t>> utils::split_data(const uint8_t* data, const size_t data_length) {
    constexpr size_t NUM_CHUNKS = 9;

    std::vector<std::vector<uint8_t>> chunks(NUM_CHUNKS);

    for (auto& chunk : chunks) {
        chunk.resize(IMAGE_CHUNK_SIZE, 0xFF);
    }

    size_t remaining_data = data_length;
    size_t data_offset = 0;

    for (size_t i = 0; i < NUM_CHUNKS && remaining_data > 0; ++i) {
        const size_t copy_size = std::min(static_cast<size_t>(IMAGE_CHUNK_SIZE), remaining_data);
        memcpy(chunks[i].data(), data + data_offset, copy_size);

        remaining_data -= copy_size;
        data_offset += copy_size;
    }

    return chunks;
}

std::string utils::wstr2str(const std::wstring &wstr) {
    std::string str(wstr.begin(), wstr.end());
    return str;
}

void utils::printHexData(const unsigned char *data, const int length) {
    for (int i = 0; i < length; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

void utils::MessageBox(const std::string& title, const std::string& message) {
    char cmd[1024];
    snprintf(cmd, 1024, R"(zenity --error --text="%s" --title="%s")", message.c_str(), title.c_str());
    FILE* fp = popen(cmd, "r");
    pclose(fp);
}

std::string utils::openFileDialog(const char* filters) {
    char path[1024] = {};
    char cmd[1024];
    snprintf(cmd, 1024, "zenity --file-selection --file-filter=%s --title \"Select your cheat shared library\"", filters);
    FILE* fp = popen(cmd, "r");
    fgets(path, sizeof(path), fp);
    char* newline = strchr(path, '\n');
    if (newline != nullptr)
        *newline = 0;
    pclose(fp);
    return {path};
}