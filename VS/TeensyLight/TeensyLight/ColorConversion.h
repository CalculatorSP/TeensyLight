# pragma once

#include <stdint.h>

struct ColorRGBW
{
public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;

    ColorRGBW(uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
    ColorRGBW(uint8_t red, uint8_t green, uint8_t blue);
};