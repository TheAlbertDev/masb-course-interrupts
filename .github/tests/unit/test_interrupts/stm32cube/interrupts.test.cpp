#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stdexcept>
#include <stdio.h>

// STM32Cube app functions prototypes
extern "C" {
#include "main.h"
extern void setup(void);
extern void loop(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
}

TEST_GROUP(Interrupts){};

TEST(Interrupts, Toggle_LED_on_interrupt_loop) {
  for (int i = 0; i < 5; ++i) {
    mock().expectNoCall("HAL_GPIO_ReadPin");

    mock()
        .expectOneCall("HAL_GPIO_WritePin")
        .withParameter("GPIOx", LED_GPIO_Port)
        .withParameter("GPIO_Pin", LED_Pin)
        .withParameter("PinState",
                       (i % 2 == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    loop();

    mock().checkExpectations();
    mock().clear();

    HAL_GPIO_EXTI_Callback(PUSH_BUTTON_Pin); // Simulate the interrupt by
                                             // calling the ISR directly
  }
}