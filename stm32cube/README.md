# Interrupts

<img align="left" src="https://img.shields.io/badge/Lab_session-2--B-yellow"><img align="left" src="https://img.shields.io/badge/Environment-STM32Cube-blue"><img align="left" src="https://img.shields.io/badge/Estimated_duration-2.5 h-green"></br>

In this second part of the second practice, we are going to **migrate the Arduino code to STM32Cube**. This time, we will make the **LED blink without using the `HAL_Delay` function** and see how to **implement an interrupt for a GPIO** using the STM32CubeMX graphical tool. The great flexibility provided by low-level register programming makes implementing an interrupt more complicated than in Arduino. **More complicated does not mean difficult**, as we will see below. Moreover, we will also see that this greater freedom opens up more options for developing our applications.

## Objectives

- Implementing interrupts on digital pins with STM32Cube.
- Alternatives to implementations based on the `HAL_Delay` function.

## Procedure

### "_Blink the LED, HAL_"

Let's first create a Git branch for development called `stm32cube-scheduling`. Below are the commands one last time.

```bash
git switch main
git pull
git switch -c stm32cube-scheduling
```

Next, we create an STM32Cube project called `scheduling`. You know how and where to do this by now 😉

We will configure pin `PA5` as a GPIO output and label it as `LED`. Save the **microcontroller configuration file** and **generate the code**.

We already know the implementation strategy since we've used it in Arduino: instead of implementing _delays_ between LED state changes, we will **schedule those state changes**.

We create the three variables we will use: `periodMillis`, `currentMillis`, and `previousMillis`. We declare them as global variables. Always make sure to **add the code where the code generator's comments indicate**!

```c
...

/* USER CODE BEGIN PV */
const uint32_t periodMillis = 1000; // period between LED toggles
uint32_t currentMillis = 0;         // current ms
uint32_t previousMillis = 0;        // ms elapsed in the previous toggle
/* USER CODE END PV */

...
```

Next, we implement the LED state change programming inside the _while loop_ of the `main` function. We will use the `HAL_GetTick` function, which is the equivalent of `millis` in Arduino.

> [!NOTE]
> Even though I mention the HAL functions to use, don't miss the opportunity to search for them in the manufacturer's HAL documentation. In your future professional life, especially during projects, you won’t be told which HAL functions to use, and you’ll need to know how to find them yourself. **Don’t miss the chance to practice searching through the documentation!**

The code would look like this. You’ll notice "something" that may seem odd in the following code. Do it as instructed, and we’ll discuss what it is later.

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

In case anyone is skeptical 😒, what should stand out to you is that **we've placed code in an improper location**. We did this on purpose so that later on, we can observe its consequences and how to fix them.

We build the code and debug. If everything is correct, we shouldn’t have any compilation errors, and the LED should be blinking every 1 second. If it works as expected, make a commit, and push your changes. Do not create the Pull Request yet.

#### _Scope_ of Variables

Before moving on to interrupts with the button, let’s look at an aspect related to the _scope_ of the variables we’ve created. Right now, we have all the variables declared as global variables. That means they are declared outside of any function, making them available to any function in the `main.c` file. The question is: **Is it necessary for the variables to be available to all functions?**

I’ll answer that for you: **no**. All the variables we have are only used within the `main` function. **We should try to avoid giving variables a larger _scope_ than necessary. In other words, we should avoid declaring variables as global if we don’t need them to be.**

In this case, we will declare the variables inside the `main` function: we remove the declarations of the variables where they are now and **move them to the beginning of the `main` function**. It will look like this.

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

We build again and check that everything works correctly.

Make another commit for the changes related to the variable scope adjustment.

#### Generate Code and Recover Version

Alright. Let's suppose we wanted to change some aspect of the microcontroller configuration. We would do this by modifying the configuration in STM32CubeMX and then clicking Save and Generate Code. In this case, we don't need to change anything, but go ahead and click Generate Code as if you had modified something. We go to the `main.c` file and... ta-daáááán! ✨ The code disappeared because we placed it in the wrong spot. Let's quickly see how to recover it.

There are a thousand and one scenarios where we might want to undo something: we've modified something and want to revert it to the version from the last _commit_, we've already made a _commit_ and want to undo it, we want to undo changes in many files, we want to undo changes in just one file, ... We need to keep this in mind because now we will see **how to manage one of those infinite scenarios: restoring the version of the last _commit_ of a single file**. This is our current scenario. We've made a _commit_, we've regenerated the code, the `main.c` file has been modified, and we want to discard the changes and leave it as it was in the last _commit_ (the one we made before regenerating the code).

Well, the command is simple:

```bash
git restore stm32cube/workspace/scheduling/Core/Src/main.c
```

The command to use is `git restore`. We simply need to specify, along with the command, the name of the file whose version we want to restore. We could also specify, instead of a file, all files in the local repository using a period `.`.

```bash
git restore .
```

But it's much better to only restore what we need. If in our case we restored everything, we would lose the `.ioc` microcontroller configuration we just created, and we'd have to do it again.

After running the `git restore` command, we go to `main.c` and see that the file has been restored to the last "_committed_" version. We fix the error and move the code to the correct location. We regenerate the code with STM32CubeMX so that it adds the necessary modifications to `main.c` and _solved_!

If it works as expected, make a commit, push your changes, and create a Pull Request to the main branch. Wait for the test results. If there are any issues, fix them, and once all tests pass, merge the Pull Request.

### Interrupts with STM32CubeMX

Now, we are going to create an interrupt for the button on the EVB so that the LED state is toggled each time we press the button. First, create a git branch named `stm32cube-interrupts`, and then create a STM32Cube project named `interrupts`. Then, we set `PUSH_BUTTON` as the _label_ for pin `PC13`. Now, for the pin's function, **we don't choose `GPIO_Input`** as we did in the previous. We choose `GPIO_EXTI13`.

![](/.github/images/stm32cube-interrupt-config.png)

Next, we are going to enable the pin interrupt. We do this by going to `System view`, at the top of the interactive image of the microcontroller, and entering `NVIC` under the `System Core` column. In this window, we can see all the interrupts available based on the current configuration of the microcontroller.

> [!NOTE]
> Only interrupts that are available for a given configuration are shown. Interrupts for peripherals that are not enabled or used will not appear. If we enabled them, new interrupts would show up.

We check the `Enabled` box for the `EXTI line[15:10] interrupts`. **This interrupt is common to pins 10 through 15**. Let's take a moment to explain how GPIO interrupts work: EXTI.

![](/.github/images/stm32cube-nvic-config.png)

#### EXTI Interrupts

The [documentation](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf) of the microcontroller describes how the interrupt module for GPIOs, called EXTI, works. If you want (and should) take a look at it, the description of the module can be found on page 202 of the [reference manual](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf).

The module generates up to 23 interrupt signals or IRQs: `EXTI[0:22]`. **All pins with the same number, regardless of the port, share the same IRQ**. For example, all pins labeled `0` (`PA0`, `PB0`, `PC0`, etc.) share the same IRQ: `EXTI0`. The other IRQs in the EXTI module behave the same way. Conclusion: **We cannot trigger an interrupt on two different pins if they share the same number**. Examples of interrupt combinations **without issues**: `PA1` and `PB4`, `PC4` and `PC3`, or `PA2` and `PD1`. Examples of interrupt combinations **with issues**: `PA1` and `PB1`, `PC5` and `PD5`, or `PA7` and `PE7`. We can enable interrupts on one or more pins, but there should not be more than one interrupt for a given pin number, regardless of the port.

![](/.github/images/exti.png)

> Image from [STMicroelectronics](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf).

Finally, we can take a look at Table 38 of the [document](https://www.st.com/content/ccc/resource/technical/document/reference_manual/5d/b1/ef/b2/a1/66/40/80/DM00096844.pdf/files/DM00096844.pdf/jcr:content/translations/en.DM00096844.pdf), where the interrupt vectors are listed. We see that `EXTI0`, `EXTI1`, and `EXTI2`, for example, have their own interrupt vector. However, **signals `EXTI` from 10 to 15 share an interrupt vector**. This means that in the first cases, we will have an ISR for a single pin, whereas in the second case, there will be one ISR for six pins. It will be **our task**, within this last common ISR for six pins, **to determine which IRQ/EXTI triggered the interrupt** in the program code and act accordingly.

#### Looking for the _Callback_...

We begin by looking at what needs to be implemented in the `main` function. As in Arduino, we will simply create a boolean variable `stateLED` which we will use to control the LED through the `HAL_GPIO_WritePin` function.

Later, in a button interrupt, we will invert the value of the boolean variable. Here's what the code will look like within the `main` function in the `main.c` file.

```c
...

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

...

  /* USER CODE BEGIN 2 */
  bool stateLED = false;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, stateLED);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */

...
```

Now it's time to see how/where STM32CubeMX implements the interrupts... Hold on to your seat...

> [!NOTE]
> In the HAL documentation, the functions or _callbacks_ that need to be used for each interrupt and module are listed directly. Here, we'll manually search for those _callbacks_, but in the future, we'll go straight to the documentation to find them.
>
> A _callback_ is simply a function called from an ISR.

We will start by going to the file `stm32f4xx_it.c`, which you will find in the same folder as the `main.c` file. At the very bottom of this file, we have the function `EXTI15_10_IRQHandler`. **The code comments are the ones that indicate that this is the function we are looking for.**

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

This function is executed when an interrupt occurs on any of the pins numbered from 10 to 15, and, of course, the interrupts need to be enabled. Inside, we have the function `HAL_GPIO_EXTI_IRQHandler`. If we click on it while pressing <kbd>CTRL</kbd> or <kbd>⌘</kbd>, we can go to the file `stm32f4xx_hal_gpio.c` and see the definition of this function.

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

There we find the function `HAL_GPIO_EXTI_Callback`. If we click on it while pressing <kbd>CTRL</kbd> or <kbd>⌘</kbd>, it takes us to its definition (which is right below). This function `HAL_GPIO_EXTI_Callback` does nothing and has the **`__weak` qualifier**. This qualifier **allows us to write another function elsewhere with the same name, which will override this one**. This is the function we will implement for our interrupt.

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

"But oh no! We're jumping from function to function without seeing anything!" I know... And the truth is... there's nothing to see! STM32CubeMX generates a series of functions that call each other and that's it. One might ask: "Why don't we call `HAL_GPIO_EXTI_Callback` directly?" STM32CubeMX uses this strategy of calling multiple functions to provide a higher level of abstraction to the code. (Do you remember what a code abstraction level is?) It's an advanced topic that's hard to grasp without prior experience in microcontroller programming, so we won't dive into it. But we should take 2 seconds to thank STM32CubeMX for doing this work for us 🙏

As we just said, we are going to create the function `HAL_GPIO_EXTI_Callback`. We will create it in the file `main.c`. There, we toggle the value of the variable `stateLED`. Below is the code.

```c
...

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  // function for the GPIO interrupt

  // toggle the LED state
  stateLED = !stateLED;
}
/* USER CODE END 4 */

...
```

Since we are using the variable `stateLED` in different functions, **we will make it a global variable**. Additionally, if we **modify** the variable `stateLED` from **different points in the program**, we will add the `volatile` qualifier to the variable.

```c
...

/* USER CODE BEGIN PV */
volatile bool stateLED = false;
/* USER CODE END PV */

...
```

Remember to remove the old delcaration of `stateLED` just before the main while loop.

Perfect! We compile, debug, and test, and everything should be fine 👌🏻

Since we have a working version of the code... You know...: make a commit, push your changes, and create a Pull Request to the main branch. Wait for the test results. If there are any issues, fix them, and once all tests pass, merge the Pull Request.

## Challenge

Just like with Arduino: make the LED toggle between off and blinking (every 500 ms) using the B1 button interrupt, without using the `HAL_Delay` and `HAL_GPIO_ReadPin` functions. There should be no restriction on when or how long B1 is pressed. It should work smoothly 😎

For the challenge, create a git branch named `stm32cube-challenge` and a project named `challenge`.

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

In this practice, we learned how to program events at the register level using the `HAL_GetTick` function. This way, we can avoid blocking the program execution during wait times created with `HAL_Delay`.

We also saw how to create an interrupt for a GPIO with STM32CubeMX and how it implements them. Using the interrupt, we avoid having to poll the button GPIO, saving microcontroller resources and ensuring that no button presses are missed, no matter how or when they occur.

In the next practice, we will move on to another peripheral and explore the use of _timers_ or counters!
