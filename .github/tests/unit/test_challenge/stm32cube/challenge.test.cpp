#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stdexcept>
#include <stdio.h>

// STM32Cube app functions prototypes
extern "C" {
#include "main.h"
extern void app(void);
extern void loop(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
}

TEST_GROUP(Challenge){};

TEST(Challenge, Toggle_LED_blinking_on_interrupt_loop) {
  uint32_t millis = 0;

  // No blink
  for (int i = 0; i < 5; ++i) {
    mock().expectNoCall("HAL_GPIO_ReadPin");
    mock().expectNoCall("HAL_Delay");

    mock().ignoreOtherCalls();

    SPY_setCurrentTicks(millis);

    loop();

    millis += 1000;

    mock().checkExpectations();
    mock().clear();
  }

  HAL_GPIO_EXTI_Callback(); // Simulate the interrupt by calling the ISR
                            // directly

  // Blink
  for (int i = 0; i < 5; ++i) {
    mock().expectNoCall("HAL_GPIO_ReadPin");
    mock().expectNoCall("HAL_Delay");

    mock().ignoreOtherCalls();

    SPY_setCurrentTicks(millis);

    loop();

    millis += 1000;

    mock().checkExpectations();
    mock().clear();
  }

  HAL_GPIO_EXTI_Callback(); // Simulate the interrupt by calling the ISR
                            // directly

  // No blink
  for (int i = 0; i < 5; ++i) {
    mock().expectNoCall("HAL_GPIO_ReadPin");
    mock().expectNoCall("HAL_Delay");

    mock().ignoreOtherCalls();

    SPY_setCurrentTicks(millis);

    loop();

    millis += 1000;

    mock().checkExpectations();
    mock().clear();
  }
}