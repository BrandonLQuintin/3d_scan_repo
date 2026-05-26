#include "OV7670_math.h"
#include "OV7670.h"

uint16_t brightest_pixels[OV7670_QVGA_HEIGHT] = {0};

typedef struct {
    uint8_t least_brightest_red;
    uint16_t best_x[5];
} red_pixel_scans_t;

static uint16_t get_pixel(uint32_t *frameBuffer, int x, int y) {
    int pixel_index = y * RESOLUTION_X + x;
    // uint32_t packs 2 RGB565 pixels in one, so we must determine where our uint16_t pixel is.
    // In other words, is our pixel in the first 2 bytes or the last 2 bytes of uint32_t?
    uint32_t word = frameBuffer[pixel_index / 2];
    return (pixel_index & 1) ? (word >> 16) & 0xFFFF : word & 0xFFFF;
}

static uint8_t get_red(uint16_t pixel) {
    return (pixel >> 11) & 0x1F;
}

static void compare_red_pixels(red_pixel_scans_t *input, uint8_t current_red_pixel, uint16_t current_x) {
    if (current_red_pixel > input->least_brightest_red) {
        input->least_brightest_red = current_red_pixel;
        for (int i = (sizeof(input->best_x) / sizeof(input->best_x[0])) - 1; i >= 1; i--) {
            input->best_x[i] = input->best_x[i - 1];
        }
        input->best_x[0] = current_x;
    }
}

void ov7670_find_brightest_pixels(uint32_t *frameBuffer, bool isSecondHalf) {
    uint8_t offset = (isSecondHalf) ? RESOLUTION_Y / 2 : 0;
    for (int y = 0; y < RESOLUTION_Y / 2; y++) {
        red_pixel_scans_t top_red_pixels = {0};
        uint16_t best_x = 0;
        for (int x = 0; x < RESOLUTION_X; x++) {
            uint8_t red = get_red(get_pixel(frameBuffer, x, y));
            compare_red_pixels(&top_red_pixels, red, x);
        }

        uint8_t array_size = sizeof(top_red_pixels.best_x) / sizeof(top_red_pixels.best_x[0]);
        best_x = top_red_pixels.best_x[array_size / 2];
        brightest_pixels[offset + y] = best_x;
    }
}