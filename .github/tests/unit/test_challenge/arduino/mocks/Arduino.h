#ifndef Arduino_H__
#define Arduino_H__

#include <stdint.h>

#define OUTPUT 0x1
#define INPUT 0x0
#define LOW 0x0
#define HIGH 0x1
#define CHANGE 0x2
#define FALLING 0x3
#define RISING 0x4

typedef void (*callback_function_t)(void);

void delay(uint32_t ms);
unsigned long millis(void);
void digitalWrite(uint32_t dwPin, uint32_t dwVal);
void pinMode(uint32_t dwPin, uint32_t dwMode);
int digitalRead(uint32_t ulPin);
void attachInterrupt(uint32_t pin, callback_function_t callback, uint32_t mode);
void detachInterrupt(uint32_t pin);
uint32_t digitalPinToInterrupt(uint32_t pin);
callback_function_t SPY_getStoredInterruptCallback(void);
void SPY_setCurrentMillis(uint32_t millis);

#endif /* Arduino_H__ */