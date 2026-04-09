#ifndef OV7670_MATH_H_
#define OV7670_MATH_H_
#include <stdint.h>
#include <stdbool.h>

extern uint16_t brightest_pixels[];

void ov7670_findBrightestPixels(uint32_t* frameBuffer, bool isSecondHalf);

#endif