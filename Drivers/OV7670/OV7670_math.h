#ifndef OV7670_MATH_H_
#define OV7670_MATH_H_
#include <stdint.h>

extern uint8_t brightest_pixels[];

void ov7670_findBrightestPixels(uint32_t* frameBuffer);

#endif