#include "stepper.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>

uint8_t stepper_states[8] = {
  0b1000,
  0b1100,
  0b0100,
  0b0110,
  0b0010,
  0b0011,
  0b0001,
  0b1001
};

void set_stepper_pin(uint8_t pin, uint8_t toggle, stepper_t *step) {
    HAL_GPIO_WritePin(step->gpio, step->pins[pin], toggle ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void set_stepper_state(uint8_t state, stepper_t *step) {
    for (int i = 3; i >= 0; i--) {
        set_stepper_pin(i, (stepper_states[state] >> i) & 1U, step);
    }
}

void clear_stepper_state(stepper_t *step) {
    for (int i = 3; i >= 0; i--) {
        set_stepper_pin(i, 0, step);
    }
}

void move_stepper(uint16_t steps, uint8_t delay, uint8_t direction, stepper_t *step) {
    if (direction == RIGHT_DIRECTION){
        step->state = 0;
    }
    if (direction == LEFT_DIRECTION){
        step->state = 7;
    }

    for (int i = 0; i < steps; i++){
        set_stepper_state(step->state, step);
        HAL_Delay(delay);

        if (direction == RIGHT_DIRECTION){
            step->state += 1;
            if (step->state > 7){
                step->state = 0;
            }
        }

        if (direction == LEFT_DIRECTION){
            step->state -= 1;
            if (step->state < 0){
                step->state = 7;
            }
        }
    }
    clear_stepper_state(step);
}