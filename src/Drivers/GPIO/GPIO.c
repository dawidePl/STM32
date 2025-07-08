#include "Drivers/GPIO/GPIO.h"

void initialize_gpio_pin_output(GPIO_Bus_t bus, uint8_t pin_number);
void initialize_gpio_pin_input(GPIO_Bus_t bus, uint8_t pin_number);

volatile GPIO_Typedef* get_bus(GPIO_Bus_t bus) {
    switch(bus) {
        case GPIOA_en: return GPIOA;
        case GPIOB_en: return GPIOB;
        case GPIOC_en: return GPIOC;
        case GPIOD_en: return GPIOD;
        default: return ((void*)0);
    }
}

/*
    Busses A, B, C wont be initialized until they are used.
    Bus initialization (for now?) basically means turning on given bus clock.

    @param GPIO_Bus_t bus Bus to enable.
*/

void initialize_gpio_bus(GPIO_Bus_t bus) {
    if(bus > GPIOD_en) return;

    enable_gpio_clock(bus);
}


void initialize_gpio_pin(GPIO_Bus_t bus, uint8_t pin_number, bool input) {
    // First check bus sanity
    if(bus > GPIOD_en) return; // Handle incorrect bus

    // Check if given bus is turned on, if not - turn it on
    if(((RCC_AHB1ENR >> bus) & 0x1) != 0x1)
        initialize_gpio_bus(bus);

    if(input == false)
        initialize_gpio_pin_output(bus, pin_number);
    else
        initialize_gpio_pin_input(bus, pin_number);

    return;
}

void initialize_gpio_pin_output(GPIO_Bus_t bus, uint8_t pin_number) {
    volatile GPIO_Typedef* gpio = get_bus(bus); // Get GPIO pin
    
    // Set GPIOA MODE register to GPOM (General Purpose Output Mode)
    gpio->MODER &= ~(0x3 << (pin_number * 2)); // Reset register bits
    gpio->MODER |= (0x1 << (pin_number * 2)); // Set "general purpose output mode"

    // Set GPIOA Output Type register, bits [31:16] RESERVED, set OTYPER to push-pull (0x0)
    gpio->OTYPER &= ~(0x1 << pin_number); // Reset register bits, since reset sets bit to 0, no need for 2nd instruction, as above

    // Not really needed, but set GPIOA OSPEED register to medium speed (0b01)
    gpio->OSPEEDR &= ~(0x3 << (pin_number * 2));
    gpio->OSPEEDR |= (0x1 << (pin_number * 2));
}

// TODO: implement, not needed now
void initialize_gpio_pin_input(GPIO_Bus_t pin, uint8_t pin_number) {
    return;
}

/*
    Sets given GPIO pin. Writing to upper 16 bits [31:16] resets given pin, writing tolower 16 bits [15:0] sets given pin.

    @param GPIO_Bus_t bus Selects bus to write to.
    @param uint8_t pin_number Selects pin to write to.
    @param bool value If 0 resets BSSR bit, if 1 sets. (0 = LOW, 1 = HIGH)
*/
void gpio_set(GPIO_Bus_t bus, uint8_t pin_number, bool value) {
    // Bus sanity check
    if(bus > GPIOD_en) return;

    // pin sanity check
    if(bus == GPIOA_en || bus == GPIOB_en) {
        if(pin_number > 15)
            return;
    }else if(bus == GPIOC_en) {
        if(pin_number < 13 || pin_number > 15)
        return;
    }

    volatile GPIO_Typedef* gpio = get_bus(bus);

    if(((gpio->MODER >> (pin_number * 2)) & 0x3) != 0x1)
        initialize_gpio_pin(bus, pin_number, false);
    
    // Reset pin
    if(value == 0)
        gpio->BSRR |= (1 << (pin_number + 16));
    else
        gpio->BSRR |= (1 << pin_number); // Set pin

    return;
}