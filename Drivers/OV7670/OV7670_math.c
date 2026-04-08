#include "OV7670_math.h"
#include "OV7670.h"

uint8_t brightest_pixels[OV7670_QVGA_HEIGHT / 2] = {0};

static uint16_t getPixel(uint32_t* frameBuffer, int x, int y) {
    int pixel_index = y * RESOLUTION_X + x;
    // uint32_t packs 2 RGB565 pixels in one, so we must determine where our uint16_t pixel is.
    // In other words, is our pixel in the first 2 bytes or the last 2 bytes of uint32_t?
    uint32_t word = frameBuffer[pixel_index / 2];
    return (pixel_index & 1) ? (word >> 16) & 0xFFFF : word & 0xFFFF;
}

static uint8_t getRed(uint16_t pixel) {
    return (pixel >> 11) & 0x1F;
}

void ov7670_findBrightestPixels(uint32_t* frameBuffer){
    for (int y = 0; y < RESOLUTION_Y / 2; y++){
        uint8_t maxRed = 0;
        uint8_t bestX = 0;
        for (int x = 0; x < RESOLUTION_X; x++){
            uint8_t red = getRed(getPixel(frameBuffer, x, y));
            if (red > maxRed) {
                maxRed = red;
                bestX = x;
            }
        }
        brightest_pixels[y] = bestX;
    }
}