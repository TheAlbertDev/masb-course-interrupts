# Interrupts

<img align="left" src="https://img.shields.io/badge/Lab_session-2--A-yellow"><img align="left" src="https://img.shields.io/badge/Environment-Arduino-blue"><img align="left" src="https://img.shields.io/badge/Estimated_duration-1.5 h-green"></br>

In the previous lab, we worked with GPIOs (General Purpose Input/Output) and learned how to read from and write to them. We also discovered some limitations when reading their values directly. For example, when turning the LED on and off using the B1 button, reading the button directly (_level-triggered_) caused the microcontroller to read it multiple times very quickly, making the LED turn on and off seemingly at random. To solve this, we used a boolean variable to detect edges, or _edge-triggered_ detection. Although this approach worked, **it has two major disadvantages**:

1. The microcontroller uses its limited resources to continuously check (or "poll") the value of the digital input. **This technique is called [_polling_](<https://en.wikipedia.org/wiki/Polling_(computer_science)>)**. **While polling, the microcontroller cannot perform other tasks.**
2. **If the button is pressed while the microcontroller is busy doing something else**, the press is missed, as if the button was never pressed. This was a problem in the previous lab challenge.

Solution? Interrupts.

An [**interrupt**](https://en.wikipedia.org/wiki/Interrupt) is a special signal sent to the CPU (Central Processing Unit), sometimes called an **Interrupt Request (IRQ)**, which tells the microcontroller that **an important event has occurred**. When the microcontroller receives an interrupt, it temporarily stops what it is doing and runs a special piece of code called an [**Interrupt Service Routine (ISR)**](https://en.wikipedia.org/wiki/Interrupt_handler). After the ISR finishes, the microcontroller continues running its previous code from where it left off.

In simple terms, interrupts are **events that temporarily pause the main program so the microcontroller can quickly respond to something important**. Examples of events that can generate interrupts include: a change (edge) on a digital input pin, a timer reaching zero, the end of an analog-to-digital conversion, or receiving a serial message.

In this lab, we will repeat the previous exercise, but this time using interrupts.

We will also learn a better way to make the LED blink, **without using the `delay` function**. We will start avoiding `delay` whenever possible, because it "locks" the microcontroller, preventing it from doing other tasks. The `delay` function was also the reason why button presses were sometimes missed in the previous lab.

## Objectives

- Introduce the concept of interrupts.
- Learn how to use interrupts on digital pins in Arduino.
- Explore alternatives to using the `delay` function for timing.

## Procedure

As always, before starting, clone this repository to your computer and create (or switch to) a new branch for your development called `arduino-scheduling`. You can find the detailed steps in the previous lab.

> [!NOTE]
> Notice that in the branch name we do not add the `branch` suffix as we did in the previous practice. In that practice, we did it only to help differentiate project branches during our first experience with Git.

### _Blink the LED like a Pro_

The function we will use to replace `delay` is `millis`. The `millis` function **returns the number of milliseconds that have passed since the program started running on the microcontroller**. According to the [documentation](https://www.arduino.cc/reference/en/language/functions/time/millis/), the value from `millis` **resets to 0 after about 50 days**. This is usually not a problem, but it is important to remember for long-running applications. Instead of using a delay, we will use a technique called **scheduling**.

Think of it like this: instead of setting a timer and staring at it until time is up, you check the time, do other things, and occasionally glance at the clock until the time has passed.

First, we will create our project in PlatformIO, named `scheduling`, inside the `arduino/workspace` folder of our local repository. Once created, and after making sure our EVB and programming method are properly configured, we initialize the LED pin `D13`.

```c
#include <Arduino.h>

#define LED 13

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);   // we configure the LED as a GPIO output
  digitalWrite(LED, LOW); // we configure the LED to be off by default
}

void loop()
{
  // put your main code here, to run repeatedly:
}
```

Did you notice? To make the code more readable, we created a macro called `LED` to represent the LED pin number.

Now, we will use three variables to control when the LED changes state:

- `periodMillis`: the desired time (in milliseconds) between LED state changes.
- `currentMillis`: the current value from the `millis` function.
- `previousMillis`: the value from `millis` the last time the LED changed state.

The code would look like this:

```c
#include <Arduino.h>

#define LED 13

const unsigned long periodMillis = 1000; // desired ms between LED state changes
unsigned long currentMillis = 0;         // elapsed ms obtained
unsigned long previousMillis = 0;        // elapsed ms obtained the last time

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);   // we configure the LED as a GPIO output
  digitalWrite(LED, LOW); // we configure the LED to be off by default
}

void loop()
{
  // put your main code here, to run repeatedly:
}
```

We declare these variables as [type `unsigned long`](https://docs.arduino.cc/language-reference/en/variables/data-types/unsignedLong/). Also, since `periodMillis` will not change during the program, we declare it as `const`. This keyword allows the compiler to optimize memory usage.

Let's see directly in the code how to use these variables in our application.

```c
#include <Arduino.h>

#define LED 13

const unsigned long periodMillis = 1000; // desired ms between LED state changes
unsigned long currentMillis = 0;         // elapsed ms obtained
unsigned long previousMillis = 0;        // elapsed ms obtained the last time

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);   // we configure the LED as a GPIO output
  digitalWrite(LED, LOW); // we configure the LED to be off by default
}

void loop()
{
  // put your main code here, to run repeatedly:
  currentMillis = millis(); // we capture the elapsed ms since the program started

  if (currentMillis - previousMillis >= periodMillis)
  { // if the desired period has passed

    previousMillis = currentMillis; // we store the elapsed ms since the program started

    // and toggle the LED
    if (digitalRead(LED) == LOW)
    {                          // if it is off
      digitalWrite(LED, HIGH); // turn it on
    }
    else
    {                          // if not
      digitalWrite(LED, HIGH); // turn it off
    }
  }
}
```

We try it and... _et voilà_. Our LED blinks without using the `delay` function. Unlike the previous example, the `loop` function now runs quickly and does not wait for 2 seconds each time.

Let's improve the code by using a boolean variable to keep track of the LED state. This makes the code easier to read and understand.

```c
#include <Arduino.h>

#define LED 13

const unsigned long periodMillis = 1000; // desired ms between LED state changes
unsigned long currentMillis = 0;        // elapsed ms obtained
unsigned long previousMillis = 0;       // elapsed ms obtained the last time

bool stateLED = LOW;

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);        // configure the LED as a GPIO output
  digitalWrite(LED, stateLED); // set the LED off by default
}

void loop()
{
  // put your main code here, to run repeatedly:
  currentMillis = millis(); // capture the elapsed ms since the program started

  if (currentMillis - previousMillis >= periodMillis)
  { // if the desired period has passed

    previousMillis = currentMillis; // store the elapsed ms since the program started

    // and toggle the LED
    stateLED = !stateLED; // use the negation operator to alternate the state
  }

  digitalWrite(LED, stateLED); // set the corresponding state
}
```

Try the code again. If it works as expected, make a commit, push your changes, and create a Pull Request to the main branch. Wait for the test results. If there are any issues, fix them, and once all tests pass, merge the Pull Request.

### GPIO Interrupts

Now, let's make the button work using interrupts. This way, we save resources and processing time, and we won't miss any button press, no matter when or how long it is pressed. From the `main` branch, create a new branch named `arduino-interrupts` and create a project named `interrupts`.

Standard Arduino only allows [interrupts for certain GPIO pins, not all pins](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/). In this lab, implementing a GPIO interrupt is simple, but handling interrupts for timers, ADCs, or serial communication can be more complex.

To implement an interrupt, we need three things:

1. **Interrupt vector**: A memory location that tells the microcontroller where to find the ISR for a specific interrupt. Each peripheral has its own interrupt vector.
2. **ISR (Interrupt Service Routine)**: The function that the microcontroller runs when the interrupt occurs.
3. **Event type**: The specific event that will trigger the interrupt. For GPIOs, there are three basic events:

- When the signal goes from `HIGH` to `LOW` (falling edge)
- When the signal goes from `LOW` to `HIGH` (rising edge)
- When the signal changes in either direction (any change)

In Arduino, we use the `attachInterrupt` function. To set up an interrupt on the button pin for a transition from `HIGH` to `LOW` (falling edge), the code looks like this:

```c
#include <Arduino.h>

#define LED 13
#define PUSH_BUTTON 23

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);        // configure the LED as a GPIO output
  digitalWrite(LED, LOW); // set the LED off by default

  pinMode(PUSH_BUTTON, INPUT); // configure the BUTTON as a GPIO input
  // add an interrupt to the BUTTON pin for a transition from HIGH to LOW (falling)
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), toggleStateLED, FALLING);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

void toggleStateLED()
{
  // ISR for the BUTTON
}
```

In the code, we use `digitalPinToInterrupt(PUSH_BUTTON)` to specify the interrupt vector. This function makes it easy to select the correct interrupt vector for different Arduino boards, so we don't need to look it up in the documentation.

Next, we specify the name of the function that will act as the ISR. In this case, we create a function (outside of `setup` and `loop`) called `toggleStateLED`, which will run every time the interrupt occurs.

Of the three types of GPIO interrupts, we configure the one that triggers on a high-to-low transition (falling edge), which is what happens when the button is pressed.

Next, we will implement the LED state change using a boolean variable called `stateLED`.

```c
#include <Arduino.h>

#define LED 13
#define PUSH_BUTTON 23

volatile bool stateLED = LOW;

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);   // configure the LED as a GPIO output
  digitalWrite(LED, LOW); // set the LED off by default

  pinMode(PUSH_BUTTON, INPUT); // configure the BUTTON as a GPIO input
  // add an interrupt to the BUTTON pin for a transition from HIGH to LOW (falling)
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), toggleStateLED, FALLING);
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(LED, stateLED); // set the corresponding state
}

void toggleStateLED()
{
  // ISR for the BUTTON
  stateLED = !stateLED; // change the LED state
}
```

You may have noticed the `volatile` keyword in front of the `stateLED` variable declaration. This keyword tells the compiler to always fetch the value of the variable from RAM, not from a register.

Here's why this matters: The microcontroller stores variables in RAM, but to work with them, it often copies them to faster registers. If two functions (for example, `loop` and an ISR) try to change the same variable at the same time, problems can occur. This is called a **race condition**.

For example, suppose `loop` wants to increment a variable called `varNumeric` (starting at 0). It copies `varNumeric` from RAM to a register, increments it, but before saving it back to RAM, an interrupt occurs. The ISR also increments `varNumeric`, but it reads the old value from RAM (still 0), increments it, and saves it back to RAM. When `loop` resumes, it saves its value (1) to RAM, overwriting the ISR's change. The variable should have been incremented twice (ending at 2), but instead, it is only 1. Using `volatile` helps prevent this kind of error.

**Key point: If a variable can be changed by both the main program and an ISR, declare it as `volatile` to avoid race conditions.**

Let's build the code... If you have followed the instructions to the letter, you should get a compilation error:

![](/.github/images/platformio-undeclared-function.png)

```shell
src/main.cpp: In function 'void setup()':
src/main.cpp:16:55: error: 'toggleStateLED' was not declared in this scope
   16 |   attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), toggleStateLED, FALLING);
      |                                                       ^~~~~~~~~~~~~~
*** [.pio/build/nucleo_f401re/src/main.cpp.o] Error 1
```

This is something you might not see in the Arduino IDE, but it happens here: we use the function name `toggleStateLED` in line 16, but its definition comes later in the file. The compiler reads from top to bottom and doesn't know about `toggleStateLED` yet. There are two solutions: 1) move the function definition above `setup`, or 2) add a function prototype (declaration) before `setup`. The Arduino IDE does the latter automatically, but here we need to do it ourselves. Just add the prototype for `toggleStateLED` before `setup`.

```diff
#include <Arduino.h>

#define LED 13
#define PUSH_BUTTON 23

volatile bool stateLED = LOW;

+ void toggleStateLED();

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);   // configure the LED as a GPIO output
  digitalWrite(LED, LOW); // set the LED off by default

  pinMode(PUSH_BUTTON, INPUT); // configure the BUTTON as a GPIO input
  // add an interrupt to the BUTTON pin for a transition from HIGH to LOW (falling)
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), toggleStateLED, FALLING);
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(LED, stateLED); // set the corresponding state
}

void toggleStateLED()
{
  // ISR for the BUTTON
  stateLED = !stateLED; // change the LED state
}
```

This allows the compiler to know that the `toggleStateLED` function exists, what parameters it takes, and what it returns. It doesn't need to know what the function does yet (that comes later in the file).

Try the code. If it works, commit your changes, push, and create a Pull Request.

## Challenge

Now, yes. Make the LED initially off and toggle its state between blinking (500 ms) and off using the button B1, without any influence from when the button is pressed or how long it is pressed. The challenge should be solved without any restrictions and without using the `delay` function.

Implement the challenge in a branch called `challenge` and name the project `challenge`.

## Evaluation

### Deliverables

These are the elements that should be available for evaluation:

- [ ] **Commits**
      Your remote GitHub repository must contain at least the following required commits: LED blinking, LED turning on and off with the button, and the challenge solution.

- [ ] **Challenge**
      The challenge must be solved and included with its own commit.

- [ ] **Pull Requests**
      The different Pull Requests requested throughout the practice must also be present in your repository.

## Conclusions

We have finished this first part of the second Arduino lab. In this part, we learned about interrupts for the first time, especially how to use interrupts with GPIOs. In future labs, as we explore new peripherals, we will always see how to configure and use their interrupts.

We also learned how to make the LED blink using the `millis` function instead of `delay`, allowing us to schedule its state changes. Later, we will see how to do the same using timer peripherals and their interrupts.

In the next part of this lab, we will see how to implement GPIO interrupts with STM32Cube.
