#ifndef Main_H__
#define Main_H__

#include <stdint.h>

#define LED_GPIO_Port 0x40020000
#define LED_Pin 0x0020

typedef uint32_t GPIO_TypeDef;
typedef uint32_t HAL_StatusTypeDef;

HAL_StatusTypeDef HAL_Init(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

uint32_t HAL_GetTick(void);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_Delay(uint32_t Delay);

void SPY_setCurrentTicks(uint32_t ticks);

#endif /* Main_H__ */