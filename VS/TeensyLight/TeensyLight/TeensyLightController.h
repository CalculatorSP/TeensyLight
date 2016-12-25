#pragma once

#include <stdint.h>
#include "Serial.h"

class TeensyLightController
{
    Serial* _serialPort;

    static void _cobsEncode(const uint8_t* source, uint32_t size, uint8_t* destination);

public:
    TeensyLightController();
    ~TeensyLightController();

    bool connect(const LPCWSTR comport);
    void disconnect();
    bool isConnected() const;
    void setPixel(uint8_t index, uint8_t red, uint8_t green, uint8_t blue, uint8_t white) const;
};