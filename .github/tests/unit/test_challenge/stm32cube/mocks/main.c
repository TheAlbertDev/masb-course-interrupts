#include "main.h"
#include "CppUTestExt/MockSupport_c.h"
#include <stddef.h>

#define SPY_HAL_GPIO_STATE_MAX 100

static uint32_t currentTicks = 0;
static SPY_HAL_GPIO_PinState hal_spy_gpio_state[SPY_HAL_GPIO_STATE_MAX] = {0};

uint32_t HAL_Init(void) { return 0; }
void SystemClock_Config(void) {}
void MX_GPIO_Init(void) {}
void MX_USART2_UART_Init(void) {}

uint32_t HAL_GetTick(void) {
  currentTicks = mock_c()
                     ->actualCall("HAL_GetTick")
                     ->returnUnsignedLongIntValueOrDefault(currentTicks);
  return currentTicks;
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  SPY_HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
  mock_c()
      ->actualCall("HAL_GPIO_TogglePin")
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
  return mock_c()
      ->actualCall("HAL_GPIO_ReadPin")
      ->withPointerParameters("GPIOx", GPIOx)
      ->withUnsignedIntParameters("GPIO_Pin", GPIO_Pin)
      ->returnUnsignedLongIntValueOrDefault(0);
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                       GPIO_PinState PinState) {
  SPY_HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
  mock_c()
      ->actualCall("HAL_GPIO_WritePin")
      ->withPointerParameters("GPIOx", GPIOx)
      ->withUnsignedIntParameters("GPIO_Pin", GPIO_Pin)
      ->withUnsignedIntParameters("PinState", PinState);
  return;
}

void SPY_HAL_setCurrentTicks(uint32_t ticks) { currentTicks = ticks; }

void SPY_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                           GPIO_PinState PinState) {

  for (size_t i = 0; i < SPY_HAL_GPIO_STATE_MAX; i++) {
    if (hal_spy_gpio_state[i].GPIOx == GPIOx &&
        hal_spy_gpio_state[i].GPIO_Pin == GPIO_Pin) {
      hal_spy_gpio_state[i].PinState = PinState;
      return;
    }
  }

  for (size_t i = 0; i < SPY_HAL_GPIO_STATE_MAX; i++) {
    if (hal_spy_gpio_state[i].GPIOx == NULL) {
      hal_spy_gpio_state[i].GPIOx = GPIOx;
      hal_spy_gpio_state[i].GPIO_Pin = GPIO_Pin;
      hal_spy_gpio_state[i].PinState = PinState;
      return;
    }
  }

  return;
}

GPIO_PinState SPY_HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {

  for (size_t i = 0; i < SPY_HAL_GPIO_STATE_MAX; i++) {
    if (hal_spy_gpio_state[i].GPIOx == GPIOx &&
        hal_spy_gpio_state[i].GPIO_Pin == GPIO_Pin) {
      return hal_spy_gpio_state[i].PinState;
    }
  }

  return GPIO_PIN_RESET;
}

void SPY_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {

  for (size_t i = 0; i < SPY_HAL_GPIO_STATE_MAX; i++) {
    if (hal_spy_gpio_state[i].GPIOx == GPIOx &&
        hal_spy_gpio_state[i].GPIO_Pin == GPIO_Pin) {
      hal_spy_gpio_state[i].PinState =
          (hal_spy_gpio_state[i].PinState == GPIO_PIN_SET) ? GPIO_PIN_RESET
                                                           : GPIO_PIN_SET;
      return;
    }
  }

  return;
}
