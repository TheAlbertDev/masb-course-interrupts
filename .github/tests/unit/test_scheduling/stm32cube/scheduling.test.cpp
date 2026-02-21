#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "main.h"
#include <stdexcept>
#include <stdio.h>

// STM32Cube app functions prototypes
extern "C" {
extern void app(void);
extern void loop(void);
}

TEST_GROUP(Scheduling){};

TEST(Scheduling, Blink_without_delay_loop) {
  for (int i = 0; i <= 10; ++i) {
    uint32_t elapsed_ms = i * 500;
    mock().expectNoCall("HAL_Delay");
    mock().expectOneCall("HAL_GetTick").andReturnValue(elapsed_ms);

    if (elapsed_ms % 1000 == 0 && elapsed_ms > 0) {
      mock()
          .expectOneCall("HAL_GPIO_TogglePin")
          .withParameter("GPIOx", LED_GPIO_Port)
          .withParameter("GPIO_Pin", LED_Pin);
    } else {
      mock().expectNoCall("HAL_GPIO_TogglePin");
    }

    ::loop();

    mock().checkExpectations();
    mock().clear();
  }
}