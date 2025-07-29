Basic (for now) operating system for STM32F4 board - meant to learn embedded programming outside of an emulator.

# GPIO Architecture

Each GPIO bus has it's own input mode, yet they can be set to be output too.

GPIOA bus is set to be in analog mode when set as input.
GPIOB bus is set to be in digital mode when set as input.
GPIOC bus can be set to either input modes, with PC13 pin being an exception as it's in-built LED.

# System clock

System clock by default runs only on PLL set at 168 MHz frequency. The system clock driver also exposes `delay()` function, which uses TIM2 clock with TIM5 being fallback clock. In future versions TIM2 clock will be the only clock running `delay` function.

# Power management

For now there is no power management, meaning the STM32 board runs on normal power mode, as per documentation.