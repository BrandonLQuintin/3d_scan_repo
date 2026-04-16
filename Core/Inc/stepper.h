#ifndef STEPPER_H_
#define STEPPER_H_

#include <stdint.h>
#include "main.h"

#define STEPPER_360 4098
#define STEP_INCREMENT 100

#define LEFT_DIRECTION 0
#define RIGHT_DIRECTION 1

extern uint8_t stepperStates[8];

typedef struct stepper{
    GPIO_TypeDef *gpio;

    uint8_t isMoving;
    uint8_t direction;
    uint16_t stepsLeft;
    uint16_t totalStepCounter;
    uint16_t pins[4];
    int8_t state;
} stepper_t;

void setStepperPin(uint8_t pin, uint8_t toggle, stepper_t *step);

void setStepperState(uint8_t state, stepper_t *step);

void moveStepper(uint16_t steps, uint8_t delay, uint8_t direction, stepper_t *step);

void clearStepperState(stepper_t *step);

#endif STEPPER_H_