//
// Created by taxmachine on 26/02/25.
//

#include "rgb.hpp"

RGB::RGB(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

RGB::RGB(const float *rgb) {
    this->rgb[0] = rgb[0];
    this->rgb[1] = rgb[1];
    this->rgb[2] = rgb[2];
}

void RGB::toRGB(uint8_t *r, uint8_t *g, uint8_t *b) const {
    *r = static_cast<uint8_t>(this->rgb[0] * 255.0f);
    *g = static_cast<uint8_t>(this->rgb[1] * 255.0f);
    *b = static_cast<uint8_t>(this->rgb[2] * 255.0f);
}

void RGB::toFloat(float rgb[4]) const {
    rgb[0] = static_cast<float>(this->r) / 255.0f;
    rgb[1] = static_cast<float>(this->g) / 255.0f;
    rgb[2] = static_cast<float>(this->b) / 255.0f;
    rgb[3] = static_cast<float>(this->a) / 255.0f;
}
