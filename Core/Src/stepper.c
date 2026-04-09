#include "stepper.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>

uint8_t stepperStates[8] = 
{
  0b1000,
  0b1100,
  0b0100,
  0b0110,
  0b0010,
  0b0011,
  0b0001,
  0b1001  
};

void setStepperPin(uint8_t pin, uint8_t toggle, stepper_t *step){
    HAL_GPIO_WritePin(step->gpio, step->pins[pin], toggle ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setStepperState(uint8_t state, stepper_t *step){
    for (int i = 3; i >= 0; i--){
        setStepperPin(i, (stepperStates[state] >> (i)) & 1U, step); 
    }

}

void clearStepperState(stepper_t *step){
    for (int i = 3; i >= 0; i--){
        setStepperPin(i, 0, step); 
    }
}

void moveStepper(uint16_t steps, uint8_t delay, uint8_t direction, stepper_t *step){
    if (direction == RIGHT_DIRECTION){
        step->state = 0;
    }
    if (direction == LEFT_DIRECTION){
        step->state = 7;
    }

    for (int i = 0; i < steps; i++){
        setStepperState(step->state, step);
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
    clearStepperState(step);
}