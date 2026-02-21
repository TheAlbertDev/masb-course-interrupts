#include "main.h"
#include "CppUTestExt/MockSupport_c.h"

static uint32_t currentTicks = 0;

uint32_t HAL_Init(void) { return 0; }
void SystemClock_Config(void) {}
void MX_GPIO_Init(void) {}
void MX_USART2_UART_Init(void) {}

uint32_t HAL_GetTick(void) {
  currentTicks = mock_c()->actualCall("HAL_GetTick")->returnUnsignedLongIntValueOrDefault(currentTicks);
  return currentTicks;
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  mock_c()->actualCall("HAL_GPIO_TogglePin")
           ->withPointerParameters("GPIOx", GPIOx)
           ->withUnsignedIntParameters("GPIO_Pin", GPIO_Pin);
  return;
}

void HAL_Delay(uint32_t Delay) {
  currentTicks += Delay;
  mock_c()->actualCall("HAL_Delay")->withUnsignedIntParameters("Delay", Delay);
  return;
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  return mock_c()->actualCall("HAL_GPIO_ReadPin")
                 ->withPointerParameters("GPIOx", GPIOx)
                 ->withUnsignedIntParameters("GPIO_Pin", GPIO_Pin)
                 ->returnUnsignedLongIntValueOrDefault(0);
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                       GPIO_PinState PinState) {
  mock_c()->actualCall("HAL_GPIO_WritePin")
           ->withPointerParameters("GPIOx", GPIOx)
           ->withUnsignedIntParameters("GPIO_Pin", GPIO_Pin)
           ->withUnsignedIntParameters("PinState", PinState);
  return;
}

void SPY_setCurrentTicks(uint32_t ticks) { currentTicks = ticks; }
