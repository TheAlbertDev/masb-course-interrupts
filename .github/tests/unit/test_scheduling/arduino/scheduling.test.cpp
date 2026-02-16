#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <stdexcept>
#include <stdio.h>
#include "Arduino.h"

// Arduino main functions prototypes
extern void setup(void);
extern void loop(void);

TEST_GROUP(Scheduling){};

TEST(Scheduling, Blink_without_delay_setup)
{
    mock()
        .expectOneCall("pinMode")
        .withParameter("dwPin", 13)
        .withParameter("dwMode", OUTPUT);
    mock()
        .expectOneCall("digitalWrite")
        .withParameter("dwPin", 13)
        .withParameter("dwVal", LOW);

    ::setup(); // Avoid calling TEST_GROUP setup() function with ::

    mock().checkExpectations();
    mock().clear();
}

TEST(Scheduling, Blink_without_delay_loop)
{
    for (int i = 0; i < 5; ++i)
    {
        mock().expectNoCall("delay");
        mock()
            .expectOneCall("millis")
            .andReturnValue(i * 1000);
        mock()
            .expectOneCall("digitalWrite")
            .withParameter("dwPin", 13)
            .withParameter("dwVal", (i % 2 == 0) ? LOW : HIGH);

        loop();

        mock().checkExpectations();
        mock().clear();
    }
}