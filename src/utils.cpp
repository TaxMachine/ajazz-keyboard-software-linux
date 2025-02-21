//
// Created by taxmachine on 17/02/25.
//

#include "utils.hpp"

#include <cstring>
#include <regex>
#include <fstream>

#include "keyboarddefs.hpp"

namespace fs = std::filesystem;

std::regex SONiX_PATTERN("usb-SONiX_AK[0-9]*-.*");

std::vector<fs::directory_entry> utils::getSONiXKeyboard() {
    std::vector<fs::directory_entry> keyboards;

    for (const auto& entry : fs::directory_iterator("/dev/input/by-id")) {
        const std::string& filename = entry.path().filename().string();

        if (std::regex_match(filename, SONiX_PATTERN)) {
            keyboards.push_back(entry);
        }
    }

    return keyboards;
}

std::pair<uint16_t, uint16_t> utils::getKeyboardIdentifiers(const fs::directory_entry& keyboard) {
    for (const auto& entry : fs::directory_iterator("/sys/class/input")) {
        fs::path device_dir = entry.path() / keyboard.path().filename() / "device";

        if (!exists(device_dir)) {
            continue;
        }

        while (exists(device_dir)) {
            if (exists(device_dir / "idVendor") && exists(device_dir / "idProduct")) {

                std::ifstream vid_file(device_dir / "idVendor");
                std::string vid_str;
                vid_file >> vid_str;

                std::ifstream pid_file(device_dir / "idProduct");
                std::string pid_str;
                pid_file >> pid_str;

                uint16_t vid = std::stoul(vid_str, nullptr, 16);
                uint16_t pid = std::stoul(pid_str, nullptr, 16);

                return {vid, pid};
            }

            device_dir = canonical(device_dir / "..");
        }
    }

    return {0, 0};
}

std::vector<std::vector<uint8_t>> split_data(const uint8_t* data, const size_t data_length) {
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