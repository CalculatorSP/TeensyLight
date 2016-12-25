#include "TeensyLightController.h"
#include "ColorConversion.h"

#define PACKETLEN       (5)
#define COBS_OVERHEAD   (1)

TeensyLightController::TeensyLightController() : _serialPort(NULL)
{ }

TeensyLightController::~TeensyLightController()
{
    disconnect();
}

bool TeensyLightController::isConnected() const
{
    return _serialPort != NULL;
}

bool TeensyLightController::connect(const LPCWSTR comport)
{
    if (isConnected())
        return false;

    try
    {
        _serialPort = new Serial(comport);
    }
    catch (...)
    {
        _serialPort = NULL;
        return false;
    }

    _serialPort->flush();
    return true;
}

void TeensyLightController::disconnect()
{
    if (!isConnected())
        return;

    delete _serialPort;
    _serialPort = NULL;
}

void TeensyLightController::setPixel(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) const
{
    ColorRGBW color(red, green, blue);

    uint8_t packet[PACKETLEN];
    packet[0] = index;
    packet[1] = color.green;
    packet[2] = color.red;
    packet[3] = color.blue;
    packet[4] = color.white;

    uint8_t encodedPacket[PACKETLEN + COBS_OVERHEAD + 1];
    encodedPacket[0] = 0x00; // delimiter
    _cobsEncode(packet, sizeof(packet), &encodedPacket[1]);

    if (isConnected())
        _serialPort->write(encodedPacket, sizeof(encodedPacket));
}

void TeensyLightController::show() const
{
    setPixel(0xFF, 0, 0, 0);
}

void TeensyLightController::_cobsEncode(const uint8_t* source, uint32_t size, uint8_t* destination)
{
    uint32_t read_index = 0;
    uint32_t write_index = 1;
    uint32_t code_index = 0;
    uint8_t code = 1;

    while (read_index < size)
    {
        if (source[read_index] == 0)
        {
            destination[code_index] = code;
            code = 1;
            code_index = write_index++;
            ++read_index;
        }
        else
        {
            destination[write_index++] = source[read_index++];
            ++code;

            if (code == 0xFF)
            {
                destination[code_index] = code;
                code = 1;
                code_index = write_index++;
            }
        }
    }

    destination[code_index] = code;
}