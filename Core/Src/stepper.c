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

void setStepperPin(uint8_t pin, uint8_t toggle, struct stepper *step){
    HAL_GPIO_WritePin(step->gpio, step->pins[pin], toggle ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setStepperState(uint8_t state, struct stepper *step){
    for (int i = 3; i >= 0; i--){
        setStepperPin(i, (stepperStates[state] >> (i)) & 1U, step); 
    }

}

void clearStepperState(struct stepper *step){
    for (int i = 3; i >= 0; i--){
        setStepperPin(i, 0, step); 
    }
}

void moveStepper(uint16_t steps, uint8_t delay, uint8_t direction, struct stepper *step){
    int8_t static state = RIGHT_DIRECTION ? 7 : 0; // <- TODO: FIX

    for (int i = 0; i < steps; i++){
        setStepperState(state, step);
        HAL_Delay(delay);

        if (direction == LEFT_DIRECTION){
            state += 1;
            if (state > 7){
                state = 0;
            }
        }

        if (direction == RIGHT_DIRECTION){
            state -= 1;
            if (state < 0){
                state = 7;
            }
        }

    
    }
    clearStepperState(step);
}