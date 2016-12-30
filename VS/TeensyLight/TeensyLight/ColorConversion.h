#pragma once

#include <stdint.h>

class ColorRGBW
{
    static const uint8_t _gamma[256];

public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;

    ColorRGBW(uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
    ColorRGBW(uint8_t red, uint8_t green, uint8_t blue);
};