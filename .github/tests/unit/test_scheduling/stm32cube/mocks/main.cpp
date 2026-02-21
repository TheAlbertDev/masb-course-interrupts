#include "main.h"
#include "CppUTestExt/MockSupport.h"

static uint32_t currentTicks = 0;

uint32_t HAL_Init(void) { return 0; }
void SystemClock_Config(void) {}
void MX_GPIO_Init(void) {}
void MX_USART2_UART_Init(void) {}

uint32_t HAL_GetTick(void) {
  MockActualCall &call = mock().actualCall("HAL_GetTick");
  currentTicks = call.returnUnsignedLongIntValueOrDefault(currentTicks);
  return currentTicks;
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  mock()
      .actualCall("HAL_GPIO_TogglePin")
      .withParameter("GPIOx", GPIOx)
      .withParameter("GPIO_Pin", GPIO_Pin);
  return;
}

void HAL_Delay(uint32_t Delay) {
  currentTicks += Delay;
  mock().actualCall("HAL_Delay").withParameter("Delay", Delay);
  return;
}

void SPY_setCurrentTicks(uint32_t ticks) { currentTicks = ticks; }
