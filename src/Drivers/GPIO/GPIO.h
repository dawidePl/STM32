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


// Pin abstraction layer
static GPIO_Typedef* const gpio_port[] = {
    GPIOA,
    GPIOB,
    GPIOC
};

#define MAKE_PIN(port_char, pin_number) ((((port_char) - 'A') << 4) | ((pin_number) & 0x0F))
#define GPIO_ABSTRACT_PIN_LIST(X) \
    X(A, 0)  X(A, 1)  X(A, 2)  X(A, 3)  X(A, 4)  X(A, 5)  X(A, 6)  X(A, 7)  X(A, 8)  X(A, 9)  X(A, 10) X(A, 11) X(A, 12) \
    X(B, 0)  X(B, 1)  X(B, 2)  X(B, 3)  X(B, 4)  X(B, 5)  X(B, 6)  X(B, 7)  X(B, 8)  X(B, 9)  X(B, 10) X(B, 11) X(B, 12) \
    X(C, 13) X(C, 14) X(C, 15)

typedef enum {
    #define X(port, pin) port##pin = MAKE_PIN(#port[0], pin),
        GPIO_ABSTRACT_PIN_LIST(X)
    #undef X
} GPIO_Pin_t;


GPIO_Typedef* get_abstracted_port(GPIO_Pin_t pin);
uint8_t get_abstracted_pin(GPIO_Pin_t pin);


// Pin modes

typedef enum {LOW = 0, HIGH = 1} GPIO_DState_t;
typedef enum {INPUT, OUTPUT} GPIO_Mode_t;


// Low-level (kernel) functions

void initialize_gpio_pin(GPIO_Pin_t pin, GPIO_Mode_t mode);

void digitalWrite(GPIO_Pin_t pin, GPIO_DState_t state);
GPIO_DState_t digitalRead(GPIO_Pin_t pin);

void analogWrite(GPIO_Pin_t pin, uint8_t value);
uint8_t analogRead(GPIO_Pin_t pin);