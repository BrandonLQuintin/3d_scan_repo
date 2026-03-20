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

void setStepperPin(uint8_t pin, uint8_t toggle){
    switch (pin) {
    case 0:
      if (toggle == 0){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
      }
      if (toggle == 1){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
      }
      break;
    case 1:
      if (toggle == 0){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
      }
      if (toggle == 1){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
      }
      break;
    case 2:
      if (toggle == 0){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
      }
      if (toggle == 1){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
      }
      break;
    case 3:
      if (toggle == 0){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
      }
      if (toggle == 1){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
      }
      break;
  }
}

void setStepperState(uint8_t state){
    for (int i = 3; i >= 0; i--){
        setStepperPin(i, (stepperStates[state] >> (i)) & 1U); 
    }

}

void clearStepperState(void){
    for (int i = 3; i >= 0; i--){
        setStepperPin(i, 0); 
    }
}

void moveStepper(uint16_t steps, uint8_t delay, uint8_t direction){
    int8_t static state = RIGHT_DIRECTION ? 7 : 0;;

    for (int i = 0; i < steps; i++){
        setStepperState(state);
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
    clearStepperState();
}