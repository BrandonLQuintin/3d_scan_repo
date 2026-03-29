Stepper Motor:
PC0 - IN1
PC1 - IN2
PC2 - IN3
PC3 - IN4
GND - GND (PSU GND + stm32f446re GND)
VCC_5V - PSU 5V
VCC_5V (VC) - Ignored

Braindumping how I could get this OV7670 camera to work with my stm32f446re...

Power & State (Camera)

3V3  -> 3.3v (DO NOT USE 5V)
GND  -> DGND
GND  -> PWDN (Tie to ground to keep camera awake)
PC5  -> RET (Camera Reset - GPIO Output)

DCMI (Video Data & Sync)

PA4  -> DCMI_HSYNC (HS)
PA6  -> DCMI_PIXCLK (PLK)
PB7  -> DCMI_VSYNC (VS)
PC6  -> DCMI_D0 (D0)
PC7  -> DCMI_D1 (D1)
PC8  -> DCMI_D2 (D2)
PC9  -> DCMI_D3 (D3)
PC11 -> DCMI_D4 (D4)
PB6  -> DCMI_D5 (D5)
PB8  -> DCMI_D6 (D6)
PB9  -> DCMI_D7 (D7)

SCCB / I2C2 (Camera Config)

PB10 -> I2C2_SCL (SCL)
PC12 -> I2C2_SDA (SDA)

Clock & PC Streaming

PA8  -> RCC_MCO_1 (XLK - Outputs the 8MHz clock to the camera)
PA2  -> USART2_TX
PA3  -> USART2_RX

Clone https://github.com/PHANzgz/STM32-H7-camera-interface-and-display
^ Use /Drivers/OV7670