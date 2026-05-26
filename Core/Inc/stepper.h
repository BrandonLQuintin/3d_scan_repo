#ifndef STEPPER_H_
#define STEPPER_H_

#include <stdint.h>
#include "main.h"

#define STEPPER_360 4076
#define STEP_INCREMENT 25

#define LEFT_DIRECTION 0
#define RIGHT_DIRECTION 1

extern uint8_t stepper_states[8];

typedef struct stepper {
    GPIO_TypeDef *gpio;

    uint8_t is_moving;
    uint8_t direction;
    uint16_t steps_left;
    uint16_t total_step_counter;
    uint16_t pins[4];
    int8_t state;
} stepper_t;

void set_stepper_pin(uint8_t pin, uint8_t toggle, stepper_t *step);
void set_stepper_state(uint8_t state, stepper_t *step);
void move_stepper(uint16_t steps, uint8_t delay, uint8_t direction, stepper_t *step);
void clear_stepper_state(stepper_t *step);

#endif