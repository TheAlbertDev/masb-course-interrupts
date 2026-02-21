# Interrupts

<img align="left" src="https://img.shields.io/badge/Lab_session-2--B-yellow"><img align="left" src="https://img.shields.io/badge/Environment-STM32Cube-blue"><img align="left" src="https://img.shields.io/badge/Estimated_duration-2.5 h-green"></br>

In this second part of the lab, we will **migrate the Arduino code to STM32Cube**. This time, we will make the **LED blink without using the `HAL_Delay` function** and learn how to **implement a GPIO interrupt** using the STM32CubeMX graphical tool. While low-level register programming in STM32Cube offers greater flexibility than Arduino, it can make interrupt implementation seem more complex. However, **more complex does not mean more difficult**—as you will see, the process is manageable and opens up new possibilities for application development.

## Objectives

- Implement interrupts on digital pins using STM32Cube.
- Explore alternatives to using the `HAL_Delay` function for timing.

## Procedure

### "_Blink the LED, HAL_"

First, create a Git branch for development called `stm32cube-scheduling`. Use the following commands:

```bash
git switch main
git pull
git switch -c stm32cube-scheduling
```

Next, create an STM32Cube project named `scheduling`. (You should already be familiar with this process.)

Configure pin `PA5` as a GPIO output and label it as `LED`. Save the **microcontroller configuration file** and **generate the code**.

Our implementation strategy, similar to what we did in Arduino, is to **schedule the LED state changes** instead of using delays. We will use three variables: `periodMillis`, `currentMillis`, and `previousMillis`. Initially, declare them as global variables. Always ensure you **add code where the code generator's comments indicate**!

```c
...

/* USER CODE BEGIN PV */
const uint32_t periodMillis = 1000; // period between LED toggles
uint32_t currentMillis = 0;         // current ms
uint32_t previousMillis = 0;        // ms elapsed in the previous toggle
/* USER CODE END PV */

...
```

Next, implement the LED state change logic inside the _while loop_ of the `main` function. Use the `HAL_GetTick` function, which is equivalent to Arduino's `millis`.

> [!NOTE]
> Although the required HAL functions are mentioned here, take this opportunity to practice searching for them in the manufacturer's HAL documentation. In your future engineering projects, you will often need to find the appropriate functions yourself. **Developing this skill now is essential!**

The code should look like this. You may notice something unusual in the following code—follow the instructions, and we will discuss it afterward.

```c
...

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    currentMillis = HAL_GetTick(); // current ms

    if (currentMillis - previousMillis >=
        periodMillis) { // if the desired period has passed

      previousMillis =
          currentMillis; // store the ms elapsed since the start of the program

      // and toggle the LED
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */

...
```

If you are wondering, what should stand out is that **we have placed code in an improper location**. This is intentional, so you can observe the consequences and learn how to fix them.

Build and debug the code. If everything is correct, there should be no compilation errors, and the LED should blink every 1 second. If it works as expected, make a commit and push your changes. Do not create the Pull Request yet.

#### Variable Scope

Before moving on to button interrupts, let's discuss the _scope_ of the variables we created. Currently, all variables are declared as global, meaning they are accessible from any function in `main.c`. But is this necessary?

The answer is **no**. These variables are only used within the `main` function. **You should avoid giving variables a broader scope than needed.** In other words, do not declare variables as global unless necessary.

In this case, move the variable declarations inside the `main` function, at its beginning. The code will look like this:

```c
...

  /* USER CODE BEGIN 2 */

  const uint32_t periodMillis = 1000; // period between LED toggles
  uint32_t currentMillis = 0;         // current ms
  uint32_t previousMillis = 0;        // ms elapsed in the previous toggle

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

...
```

Build again and verify that everything works correctly.

Make another commit for the changes related to variable scope adjustment.

#### Generating Code and Restoring Versions

Suppose you want to change some aspect of the microcontroller configuration. You would modify the configuration in STM32CubeMX, then click Save and Generate Code. In this example, you do not need to change anything, but try clicking Generate Code as if you had made a modification. Now, check the `main.c` file—notice that the code you added has disappeared because it was placed in the wrong location. Let's see how to recover it.

There are many scenarios where you might want to undo changes: perhaps you want to revert a file to its last committed version, undo a recent commit, or restore only a specific file. In our current scenario, you have made a commit, regenerated the code, and now want to discard changes in `main.c` to return it to its last committed state (before code regeneration).

The command is simple:

```bash
git restore stm32cube/workspace/scheduling/Core/Src/main.c
```

The command to use is `git restore`. We simply need to specify, along with the command, the name of the file whose version we want to restore. We could also specify, instead of a file, all files in the local repository using a period `.`.

```bash
git restore .
```

However, it is better to restore only what you need. If you restore everything, you may lose recent changes to other files, such as the `.ioc` microcontroller configuration.

After running the `git restore` command, check that `main.c` has been restored to its last committed version. Fix the error by moving your code to the correct location, then regenerate the code with STM32CubeMX so that your changes are preserved.

If it works as expected, make a commit. Next, let's organize the code in a way similar to Arduino, to avoid problems caused by placing code in the wrong locations.

#### Organizing Code: Setup and Loop

The Arduino code structure is widely used because it provides a clear separation between initialization and main application logic. There is a `setup` function that runs once at the beginning, and a `loop` function that runs repeatedly. We will manually create these functions in a new file and call them from the `main` function. This approach keeps your application code separate from STM32Cube-generated code, making it easier to maintain and less prone to accidental overwrites during code generation.

##### Creating the Application File

In the `Core/Src` folder (where `main.c` is located), create a new file called `app.c`. Add the following code:

```c
void setup(void) {}

void loop(void) {}
```

This emulates the Arduino structure. For blinking an LED, nothing is needed at the start, so `setup` is empty. However, we include it for completeness and future use.

For the `loop` function, add the following:

```c
void setup(void) {}

void loop(void) {
  const uint32_t periodMillis = 1000; // period between LED toggles
  uint32_t currentMillis = 0;         // current ms
  uint32_t previousMillis = 0;        // ms elapsed in

  currentMillis = HAL_GetTick(); // current ms

  if (currentMillis - previousMillis >=
      periodMillis) { // if the desired period has passed

    previousMillis =
        currentMillis; // store the ms elapsed since the start of the program

    // and toggle the LED
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}
```

If you try to compile now, you will get errors in `app.c` because the compiler does not know about functions like `HAL_GPIO_TogglePin`, the value of `LED_GPIO_Port`, or the type `uint32_t`. To fix this, include `main.h` at the top of `app.c` (as is already done in `main.c`). All necessary declarations are available in `main.h`.

```diff
+ #include "main.h"

void setup(void) {}

void loop(void) {
  const uint32_t periodMillis = 1000; // period between LED toggles
  uint32_t currentMillis = 0;         // current ms
  uint32_t previousMillis = 0;        // ms elapsed in

  currentMillis = HAL_GetTick(); // current ms

  if (currentMillis - previousMillis >=
      periodMillis) { // if the desired period has passed

    previousMillis =
        currentMillis; // store the ms elapsed since the start of the program

    // and toggle the LED
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}
```

Now your application code is much cleaner and easier to maintain than if it were all in `main.c`.

##### Calling `setup` and `loop` from `main`

Next, call these functions from `main.c`. First, clean up the `main` function by removing any code you have moved to `setup` and `loop`. Then, just before the while loop, call `setup()`, and inside the while loop, call `loop()`:

```c
...

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  setup();
  while (1) {
    loop();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */

...
```

If you try to compile now, the compiler will complain that these functions are not declared. To fix this, create a header file `app.h` with the function prototypes, and include it in `main.h`.

##### Creating the App Header File

Create a file named `app.h` in the `Core/Inc` folder, next to `main.h`. Add the following:

```c
#ifndef APP_H__
#define APP_H__

void setup(void);
void loop(void);

#endif /* APP_H__ */
```

As you can see, there are the prototypes for our functions. But there's more. The first two lines and the last one form an include guard. What is this for? Well, includes are used to tell the compiler about the existence of functions, variables, variable types, etc. But what happens if we need to use that include in several files at once? The compiler would receive the information multiple times and would complain, for example, that the `setup` function already exists. The way to solve this is with include guards. If you look, the include guard is an `if`. If the macro `APP_H__` doesn't exist, it creates it and returns the prototypes. If a second file includes `app.h`, since the macro `APP_H__` will already exist, it won't return the prototypes again. Very smart, right?

Now, include `app.h` in `main.c`:

```c
...

/* USER CODE BEGIN Includes */
#include "app.h"
/* USER CODE END Includes */

...
```

If you compile now, you will get an error. The compiler does not know where the definitions of our `setup` and `loop` functions are (our `app.c` file). You need to add the file path to the compiler configuration file.

##### Adding Files to CMakeLists.txt

The `CMakeLists.txt` file is used to collect all the project information (paths to source files, compiler installation paths, paths to folders containing header files, etc.). From this file, CMake generates a build launch file which, once run, performs the compilation. We need to add the path to our new `app.c` file in `CMakeLists.txt`. To do this, open the `CMakeLists.txt` file located at the root of the project (be careful to open the correct one, as there may be more than one), and look for `# Add sources to executable`. In that section, add the file path:

```cmake
...

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_SOURCE_DIR}/Core/Src/app.c
)

...
```

We have used the CMake variable `CMAKE_SOURCE_DIR` to obtain the project path directly, so the indicated path will work even if we move the project folder. That’s all you need to do.

> [!NOTE]
> If you’re wondering whether you need to do the same for the `app.h` file, you don’t. Header files (`.h`) are not specified by their file path, but rather by the path to the folder containing them. Since the `Core/Inc` folder is already included by default, there’s nothing else you need to do.

Compile and check that everything works correctly. If it does, make a commit, push your changes, and create a Pull Request to the main branch. Wait for the test results. If there are any issues, fix them, and once all tests pass, merge the Pull Request.

> [!IMPORTANT]
> From now on, use this code organization for all STM32Cube projects.

### Interrupts with STM32CubeMX

Now, let's create an interrupt for the button on the evaluation board (EVB) so that the LED toggles each time the button is pressed. First, create a git branch named `stm32cube-interrupts`, and then create an STM32Cube project named `interrupts`. Set `PUSH_BUTTON` as the label for pin `PC13`. For the pin's function, **do not choose `GPIO_Input`** as before. Instead, select `GPIO_EXTI13`.

![](/.github/images/stm32cube-interrupt-config.png)

Next, enable the pin interrupt. Go to `System view` at the top of the STM32CubeMX interface, and select `NVIC` under the `System Core` column. Here, you can see all interrupts available for the current microcontroller configuration.

> [!NOTE]
> Only interrupts available for the current configuration are shown. Interrupts for peripherals that are not enabled will not appear. If you enable more peripherals, new interrupts will show up.

Check the `Enabled` box for the `EXTI line[15:10] interrupts`. **This interrupt is shared by pins 10 through 15**. Let's take a moment to explain how GPIO interrupts work using EXTI.

![](/.github/images/stm32cube-nvic-config.png)

#### EXTI Interrupts

The [microcontroller documentation](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf) describes how the EXTI (External Interrupt) module for GPIOs works. You are encouraged to read about it—see page 202 of the [reference manual](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf).

The EXTI module generates up to 23 interrupt signals (IRQs): `EXTI[0:22]`. **All pins with the same number, regardless of port, share the same IRQ**. For example, all pins labeled `0` (`PA0`, `PB0`, `PC0`, etc.) share the same IRQ: `EXTI0`. The same applies to other pin numbers. Therefore, **you cannot trigger interrupts on two different pins if they share the same number**.

Examples of valid interrupt combinations: `PA1` and `PB4`, `PC4` and `PC3`, or `PA2` and `PD1`.
Examples of problematic combinations: `PA1` and `PB1`, `PC5` and `PD5`, or `PA7` and `PE7`.
You can enable interrupts on multiple pins, but there should not be more than one interrupt for a given pin number, regardless of port.

![](/.github/images/exti.png | width=100)

> Image from [STMicroelectronics](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf).

Finally, look at Table 38 of the [reference manual](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf), which lists the interrupt vectors. `EXTI0`, `EXTI1`, and `EXTI2` each have their own interrupt vector. However, **EXTI signals from 10 to 15 share a single interrupt vector**. This means that for pins 10–15, there is one ISR (Interrupt Service Routine) for all six pins. **It is your responsibility, within this ISR, to determine which pin triggered the interrupt and handle it accordingly.**

#### Finding the _Callback_ Function

Let's see what needs to be implemented in `app.c`. As in Arduino, we will create a boolean variable `stateLED` to control the LED using the `HAL_GPIO_WritePin` function.

Later, in the button interrupt, we will invert the value of this boolean variable. Here is what the code looks like in `app.c`:

```c
#include "main.h"
#include <stdbool.h>

bool stateLED = false;

void setup(void) {}

void loop(void) { HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, stateLED); }
```

Now, let's see how and where STM32CubeMX implements interrupts.

> [!NOTE]
> The HAL documentation lists the functions or _callbacks_ to use for each interrupt and module. Here, we will manually search for these callbacks, but in the future, you should consult the documentation directly.
>
> A _callback_ is simply a function called from an ISR.

Start by opening the file `stm32f4xx_it.c` (in the same folder as `main.c`). At the bottom of this file, you will find the function `EXTI15_10_IRQHandler`. **The code comments indicate that this is the function you are looking for.**

```c
...

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(PUSH_BUTTON_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

...
```

This function is executed when an interrupt occurs on any of the pins numbered 10 to 15 (provided the interrupts are enabled). Inside, it calls the function `HAL_GPIO_EXTI_IRQHandler`. If you follow this function (using <kbd>CTRL</kbd> or <kbd>⌘</kbd> + click), you will find its definition in `stm32f4xx_hal_gpio.c`.

```c
...

/**
  * @brief  This function handles EXTI interrupt request.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Callback(GPIO_Pin);
  }
}

...
```

There, you will find the function `HAL_GPIO_EXTI_Callback`. If you follow this function, you will see its definition (just below). This function does nothing and is marked with the **`__weak` qualifier**. This means you can write your own function with the same name elsewhere, and it will override the weak version. This is the function you will implement for your interrupt.

```c
...

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}

...
```

You may wonder why STM32CubeMX uses this chain of function calls instead of calling `HAL_GPIO_EXTI_Callback` directly. This approach provides a higher level of abstraction, making the code more flexible and maintainable. For now, just appreciate that STM32CubeMX handles much of this complexity for you.

Now, create the function `HAL_GPIO_EXTI_Callback` in `app.c`. In this function, toggle the value of `stateLED`:

```c
#include "main.h"
#include <stdbool.h>

bool stateLED = false;

void setup(void) {}

void loop(void) { HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, stateLED); }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  // function for the GPIO interrupt

  // toggle the LED state
  stateLED = !stateLED;
}
```

Since `stateLED` will be modified from different parts of the program (app code and interrupt), add the `volatile` qualifier to the variable:

```c
#include "main.h"
#include <stdbool.h>

volatile bool stateLED = false;

void setup(void) {}

void loop(void) { HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, stateLED); }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  // function for the GPIO interrupt

  // toggle the LED state
  stateLED = !stateLED;
}
```

Compile, debug, and test your code. Everything should work correctly.

Once you have a working version, make a commit, push your changes, and create a Pull Request to the main branch. Wait for the test results. If there are any issues, fix them, and once all tests pass, merge the Pull Request.

## Challenge

As in the Arduino example: make the LED toggle between off and blinking (every 500 ms) using the B1 button interrupt, **without using the `HAL_Delay` or `HAL_GPIO_ReadPin` functions**. There should be no restriction on when or how long B1 is pressed. The system should work smoothly.

Remember to use the setup/loop code organization.

For the challenge, create a git branch named `stm32cube-challenge` and a project named `challenge`.

## Evaluation

### Deliverables

The following elements are required for evaluation:

- [ ] **Commits**: Your remote GitHub repository must contain at least the following commits: LED blinking, LED toggling with the button, and the challenge solution.
- [ ] **Challenge**: The challenge must be solved and included with its own commit.
- [ ] **Pull Requests**: All requested Pull Requests throughout the lab must be present in your repository.

## Conclusions

In this lab, you learned how to schedule events at the register level using the `HAL_GetTick` function, allowing you to avoid blocking program execution with `HAL_Delay`.

You also learned how to create a GPIO interrupt with STM32CubeMX and how the tool implements them. Using interrupts, you avoid polling the button GPIO, saving microcontroller resources and ensuring that no button presses are missed, regardless of timing.

In the next lab, we will explore another peripheral and learn about using _timers_ and counters!
