#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "mocks/main.h"

#include <stdexcept>
#include <stdio.h>

// STM32Cube app functions prototypes
extern "C" {
#include "main.h"
extern void setup(void);
extern void loop(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
}

static GPIO_PinState expectedPinState = GPIO_PIN_RESET;

TEST_GROUP(Challenge) {
  void setup() {
    SPY_HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  };
};

TEST(Challenge, Toggle_LED_blinking_on_interrupt_loop) {

  expectedPinState = GPIO_PIN_RESET;

  // No blink
  for (uint32_t elapsed_ms = 0; elapsed_ms < 5000; elapsed_ms += 200) {
    mock().expectNoCall("HAL_GPIO_ReadPin");
    mock().expectNoCall("HAL_Delay");
    mock().ignoreOtherCalls();

    SPY_HAL_setCurrentTicks(elapsed_ms);

    ::loop();

    CHECK_EQUAL(expectedPinState, SPY_HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin));

    mock().checkExpectations();
    mock().clear();
  }

  mock().expectNoCall("HAL_GPIO_ReadPin");
  mock().expectNoCall("HAL_Delay");
  mock().ignoreOtherCalls();
  HAL_GPIO_EXTI_Callback(PUSH_BUTTON_Pin); // Simulate the interrupt by calling
  // the ISR directly
  expectedPinState = GPIO_PIN_SET;

  mock().checkExpectations();
  mock().clear();

  // Blink
  for (uint32_t elapsed_ms = 5000; elapsed_ms < 10000; elapsed_ms += 200) {
    mock().expectNoCall("HAL_GPIO_ReadPin");
    mock().expectNoCall("HAL_Delay");
    mock().expectOneCall("HAL_GetTick").andReturnValue(elapsed_ms);
    mock().ignoreOtherCalls();

    if (elapsed_ms % 1000 == 0 && elapsed_ms > 5000) {
      expectedPinState =
          (expectedPinState == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }

    SPY_HAL_setCurrentTicks(elapsed_ms);

    ::loop();

    CHECK_EQUAL(expectedPinState, SPY_HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin));

    mock().checkExpectations();
    mock().clear();
  }

  mock().expectNoCall("HAL_GPIO_ReadPin");
  mock().expectNoCall("HAL_Delay");
  mock().ignoreOtherCalls();
  HAL_GPIO_EXTI_Callback(PUSH_BUTTON_Pin); // Simulate the interrupt by calling
  // the ISR directly
  expectedPinState = GPIO_PIN_RESET;
  mock().checkExpectations();
  mock().clear();

  // No blink
  for (uint32_t elapsed_ms = 10000; elapsed_ms < 15000; elapsed_ms += 200) {
    mock().expectNoCall("HAL_GPIO_ReadPin");
    mock().expectNoCall("HAL_Delay");
    mock().ignoreOtherCalls();

    SPY_HAL_setCurrentTicks(elapsed_ms);

    ::loop();

    CHECK_EQUAL(GPIO_PIN_RESET, SPY_HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin));

    mock().checkExpectations();
    mock().clear();
  }
}