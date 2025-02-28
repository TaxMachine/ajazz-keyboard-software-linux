//
// Created by taxmachine on 26/02/25.
//

#ifndef RGB_HPP
#define RGB_HPP

#include <cstdint>

class RGB {
public:
    RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    explicit RGB(const float* rgb);

    void toFloat(float rgb[4]) const;
    void toRGB(uint8_t* r, uint8_t* g, uint8_t* b) const;

private:
    uint8_t r{};
    uint8_t g{};
    uint8_t b{};
    uint8_t a{};
    float rgb[4] = {};
};

#endif //RGB_HPP
