#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stdexcept>
#include <stdio.h>
#include "Arduino.h"

// Arduino main functions prototypes
extern void setup(void);
extern void loop(void);

callback_function_t interruptCallback = nullptr;

TEST_GROUP(Challenge){
    // clang-format off
    void setup()
    {
        mock()
            .expectOneCall("attachInterrupt")
            .withParameter("pin", 23)
            .withParameter("mode", FALLING)
            .ignoreOtherParameters();
            
        mock().ignoreOtherCalls();
        
        ::setup();

        // Store the interrupt callback for later use in tests
        interruptCallback = SPY_getStoredInterruptCallback();

        mock().checkExpectations();
        mock().clear();
    }
// clang-format on
}
;

TEST(Challenge, Toggle_LED_blinking_on_interrupt_loop)
{
    uint32_t millis = 0;

    // No blink
    for (int i = 0; i < 5; ++i)
    {
        mock().expectNoCall("digitalRead");
        mock().expectNoCall("delay");

        mock().ignoreOtherCalls();

        SPY_setCurrentMillis(millis);

        loop();

        millis += 1000;

        mock().checkExpectations();
        mock().clear();
    }

    interruptCallback(); // Simulate the interrupt by calling the ISR directly

    // Blink
    for (int i = 0; i < 5; ++i)
    {
        mock().expectNoCall("digitalRead");
        mock().expectNoCall("delay");

        mock().ignoreOtherCalls();

        SPY_setCurrentMillis(millis);

        loop();

        millis += 1000;

        mock().checkExpectations();
        mock().clear();
    }

    interruptCallback(); // Simulate the interrupt by calling the ISR directly

    // No blink
    for (int i = 0; i < 5; ++i)
    {
        mock().expectNoCall("digitalRead");
        mock().expectNoCall("delay");

        mock().ignoreOtherCalls();

        SPY_setCurrentMillis(millis);

        loop();

        millis += 1000;

        mock().checkExpectations();
        mock().clear();
    }
}