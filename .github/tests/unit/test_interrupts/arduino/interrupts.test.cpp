#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stdexcept>
#include <stdio.h>
#include "Arduino.h"

// Arduino main functions prototypes
extern void setup(void);
extern void loop(void);
extern void toggleStateLED(void);

TEST_GROUP(Interrupts){};

TEST(Interrupts, Toggle_LED_on_interrupt_setup)
{
    mock()
        .expectOneCall("pinMode")
        .withParameter("dwPin", 13)
        .withParameter("dwMode", OUTPUT);
    mock()
        .expectOneCall("digitalWrite")
        .withParameter("dwPin", 13)
        .withParameter("dwVal", LOW);

    mock()
        .expectOneCall("pinMode")
        .withParameter("dwPin", 23)
        .withParameter("dwMode", INPUT);

    mock().expectOneCall("digitalPinToInterrupt").withParameter("pin", 23).andReturnValue(23);
    mock().expectOneCall("attachInterrupt").withParameter("pin", 23).withParameter("callback", toggleStateLED).withParameter("mode", FALLING);

    ::setup(); // Avoid calling TEST_GROUP setup() function with ::

    mock().checkExpectations();
    mock().clear();
}

TEST(Interrupts, Toggle_LED_on_interrupt_loop)
{
    for (int i = 0; i < 5; ++i)
    {
        mock().expectNoCall("digitalRead");
        mock()
            .expectOneCall("digitalWrite")
            .withParameter("dwPin", 13)
            .withParameter("dwVal", (i % 2 == 0) ? LOW : HIGH);

        loop();

        mock().checkExpectations();
        mock().clear();

        toggleStateLED(); // Simulate the interrupt by calling the ISR directly
    }
}