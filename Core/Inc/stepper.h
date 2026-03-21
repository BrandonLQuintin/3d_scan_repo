#ifndef __STEPPER_H
#define __STEPPER_H

#include <stdint.h>
#include "main.h"

#define STEPPER_360 4098

#define LEFT_DIRECTION 0
#define RIGHT_DIRECTION 1

extern uint8_t stepperStates[8];

struct stepper{
    GPIO_TypeDef *gpio;

    uint8_t isMoving;
    uint8_t direction;
    uint16_t stepsLeft;
    uint16_t pins[4];
    int8_t state;
};

void setStepperPin(uint8_t pin, uint8_t toggle, struct stepper *step);

void setStepperState(uint8_t state, struct stepper *step);

void moveStepper(uint16_t steps, uint8_t delay, uint8_t direction, struct stepper *step);

void clearStepperState(struct stepper *step);

#endif __STEPPER_H