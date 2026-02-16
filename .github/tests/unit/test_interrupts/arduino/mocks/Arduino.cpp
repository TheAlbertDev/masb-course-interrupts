#include "CppUTestExt/MockSupport.h"
#include "Arduino.h"

void pinMode(uint32_t ulPin, uint32_t ulMode)
{
    mock()
        .actualCall("pinMode")
        .withParameter("dwPin", ulPin)
        .withParameter("dwMode", ulMode);
    return;
}

void digitalWrite(uint32_t ulPin, uint32_t ulVal)
{
    mock()
        .actualCall("digitalWrite")
        .withParameter("dwPin", ulPin)
        .withParameter("dwVal", ulVal);
    return;
}

void delay(uint32_t ms)
{
    mock()
        .actualCall("delay")
        .withParameter("ms", ms);
    return;
}

unsigned long millis(void)
{
    return mock()
        .actualCall("millis")
        .returnUnsignedLongIntValueOrDefault(0);
}

int digitalRead(uint32_t ulPin)
{
    return mock()
        .actualCall("digitalRead")
        .withParameter("ulPin", ulPin)
        .returnIntValueOrDefault(LOW);
}

void attachInterrupt(uint32_t pin, callback_function_t callback, uint32_t mode)
{
    mock()
        .actualCall("attachInterrupt")
        .withParameter("pin", pin)
        .withParameter("callback", callback)
        .withParameter("mode", mode);
    return;
}

void detachInterrupt(uint32_t pin)
{
    mock()
        .actualCall("detachInterrupt")
        .withParameter("pin", pin);
    return;
}

uint32_t digitalPinToInterrupt(uint32_t pin)
{
    return mock()
        .actualCall("digitalPinToInterrupt")
        .withParameter("pin", pin)
        .returnUnsignedIntValueOrDefault(pin);
}