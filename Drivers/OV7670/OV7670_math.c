#include "OV7670_math.h"
#include "OV7670.h"

uint16_t brightest_pixels[OV7670_QVGA_HEIGHT] = {0};

typedef struct {
    uint8_t leastBrightestRed;
    uint16_t bestX[5];
} RedPixelScans;

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

static void compareRedPixels(RedPixelScans *input, uint8_t current_red_pixel, uint8_t current_x){
    if (current_red_pixel > input->leastBrightestRed){
        input->leastBrightestRed = current_red_pixel;
        for (int i = sizeof(input->bestX) / sizeof(input->bestX[0]); i >= 1; i--) {
            input->bestX[i] = input->bestX[i - 1];
        }
        input->bestX[0] = current_x;
    }
}

void ov7670_findBrightestPixels(uint32_t* frameBuffer, bool isSecondHalf){
    uint8_t offset = (isSecondHalf) ? RESOLUTION_Y / 2 : 0;
    for (int y = 0; y < RESOLUTION_Y / 2; y++){
        RedPixelScans topRedPixels = {0};
        uint16_t bestX = 0;
        for (int x = 0; x < RESOLUTION_X; x++){
            uint8_t red = getRed(getPixel(frameBuffer, x, y));
            compareRedPixels(&topRedPixels, red, x);
        }

        // Choosing the middle of the top brightest red pixels will result in less noisy data being passed through.
        uint8_t arraySize = sizeof(topRedPixels.bestX) / sizeof(topRedPixels.bestX[0]);
        bestX = topRedPixels.bestX[arraySize / 2];
        brightest_pixels[offset + y] = bestX;
    }
}