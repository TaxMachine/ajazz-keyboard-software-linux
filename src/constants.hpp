//
// Created by taxmachine on 26/02/25.
//

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <memory>

#include "utils/config.hpp"
#include "keyboards/ak820pro.hpp"

inline std::shared_ptr<AK820Pro>& GetKeyboard() {
    static std::shared_ptr<AK820Pro> instance;
    return instance;
}

inline std::shared_ptr<Config>& GetConfig() {
    static std::shared_ptr<Config> instance;
    return instance;
}

#endif //CONSTANTS_HPP
