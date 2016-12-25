#include <Adafruit_NeoPixel.h>

#define PIN            (17)
#define NUMPIXELS      (256)
#define PACKETSIZE     (5)
#define COBS_OVERHEAD  (1)

static Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);

void setup()
{
  pixels.begin();
  pixels.clear();
  pixels.show();
  Serial.begin(9600);
}

void loop()
{
  // Wait for delimiter
  while (waitForByte() != 0x00);

  // Read COBS encoded packet
  uint8_t encodedPacket[PACKETSIZE + COBS_OVERHEAD];
  for (uint8_t i = 0; i < sizeof(encodedPacket); ++i)
    encodedPacket[i] = waitForByte();

  // Decode packet
  uint8_t decodedPacket[PACKETSIZE];
  cobs_decode(encodedPacket, sizeof(encodedPacket), decodedPacket);

  // Set pixel value
  pixels.setPixelColor(decodedPacket[0], Adafruit_NeoPixel::Color(decodedPacket[1], decodedPacket[2], decodedPacket[3], decodedPacket[4]));
  pixels.show();
}

uint8_t waitForByte()
{
  while (true)
  {
    int val = Serial.read();
    if (val >= 0)
      return (uint8_t)val;
  }
}

uint32_t cobs_decode(const uint8_t* input, uint32_t len, uint8_t* output)
{
  uint32_t read_index = 0;
  uint32_t write_index = 0;

  while (read_index < len)
  {
    uint8_t code = input[read_index];
    if (read_index + code > len && code != 1)
      return 0;

    ++read_index;

    for (uint8_t i = 1; i < code; ++i)
      output[write_index++] = input[read_index++];

    if (code != 0xFF && read_index != len)
      output[write_index++] = '\0';
  }

  return write_index;
}

