#ifndef __STEPPER_H
#define __STEPPER_H

#include <stdint.h>
#include "main.h"

#define STEPPER_360 4098

#define LEFT_DIRECTION 0
#define RIGHT_DIRECTION 1

extern uint8_t stepperStates[8];

void setStepperPin(uint8_t pin, uint8_t toggle);

void setStepperState(uint8_t state);

void moveStepper(uint16_t steps, uint8_t delay, uint8_t direction);

void clearStepperState(void);

#endif __STEPPER_H