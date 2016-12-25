#include "ColorConversion.h"
#include <algorithm>

ColorRGBW::ColorRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) : red(r), green(g), blue(b), white(w)
{ }

ColorRGBW::ColorRGBW(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t minval = std::min(r, std::min(g, b));

    red = r - minval;
    green = g - minval;
    blue = b - minval;
    white = minval;
}