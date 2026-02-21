#include "CppUTestExt/MockSupport.h"
#include "Arduino.h"

// Static variable to store the interrupt callback
static callback_function_t stored_interrupt_callback = nullptr;
static uint32_t currentMillis = 0;

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
    MockActualCall &call = mock().actualCall("millis");
    currentMillis = call.returnUnsignedLongIntValueOrDefault(currentMillis);
    return currentMillis;
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
    stored_interrupt_callback = callback;

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

callback_function_t SPY_getStoredInterruptCallback(void)
{
    return stored_interrupt_callback;
}

void SPY_setCurrentMillis(uint32_t millis)
{
    currentMillis = millis;
}