#pragma once

#include "Generic/generic.h"
#include "Drivers/SYSCLK/SYSCLK.h"

#define GPIOA_BASE 0x40020000UL
#define GPIOB_BASE 0x40020400UL
#define GPIOC_BASE 0x40020800UL
#define GPIOD_BASE 0x40020C00UL

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
} GPIO_Typedef;


#define GPIOA ((GPIO_Typedef *) GPIOA_BASE)
#define GPIOB ((GPIO_Typedef *) GPIOB_BASE)
#define GPIOC ((GPIO_Typedef *) GPIOC_BASE)
#define GPIOD ((GPIO_Typedef *) GPIOD_BASE)


void initialize_gpio_bus(GPIO_Bus_t bus);
void initialize_gpio_pin(GPIO_Bus_t bus, uint8_t pin_number, bool input);

void gpio_set(GPIO_Bus_t bus, uint8_t pin_number, bool value);