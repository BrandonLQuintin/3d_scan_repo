
Stepper Motor:
PC0 - IN1
PC1 - IN2
PC2 - IN3
PC3 - IN4
GND - GND (PSU GND + stm32f446re GND)
VCC_5V - PSU 5V
VCC_5V (VC) - Ignored

Braindumping how I could get this OV7670 camera to work with my stm32f446re...

DCMI

PA4  -> DCMI_HSYNC (HREF)
PA6  -> DCMI_PIXCLK (PCLK)
PB7  -> DCMI_VSYNC
PC6  -> DCMI_D0
PC7  -> DCMI_D1
PC8  -> DCMI_D2
PC9  -> DCMI_D3
PC11 -> DCMI_D4
PB6  -> DCMI_D5
PB8  -> DCMI_D6
PB9  -> DCMI_D7

SCCB / I2C2

PB10 -> I2C2_SCL (SIOC)
PC12 -> I2C2_SDA (SIOD)

Clock & PC Streaming

PA8  -> RCC_MCO_1 (Outputs the 8MHz clock to the camera)
PA2  -> USART2_TX
PA3  -> USART2_RX

Camera Reset

PC5 -> Camera Reset (GPIO Output)

Clone https://github.com/PHANzgz/STM32-H7-camera-interface-and-display
^ Use /Drivers/OV7670