#include "Drivers/GPIO/GPIO.h"


GPIO_Typedef* get_abstracted_port(GPIO_Pin_t pin) {
    uint8_t port_index = (pin >> 4) & 0x0F;
    
    if(port_index < sizeof(gpio_port) / sizeof(gpio_port[0]))
        return gpio_port[port_index];

    return (void*)(0);
}

uint8_t get_abstracted_pin(GPIO_Pin_t pin) {
    return pin & 0x0F;
}

uint8_t get_port_index(GPIO_Pin_t pin) {
    return (pin >> 4) & 0x0F;
}



/*
    Turn on bus clock for given port.
*/
void initialize_gpio_port(GPIO_Pin_t pin) {
    uint8_t port_index = get_port_index(pin);

    enable_gpio_clock(port_index);
}


void initialize_gpio_pin(GPIO_Pin_t pin, GPIO_Mode_t mode) {
    uint8_t port_index = get_port_index(pin);
    uint8_t pin_number = get_abstracted_pin(pin);
    GPIO_Typedef* gpio = get_abstracted_port(pin);

    if(!gpio) return; // TODO: use PC13 (in-built LED) to signal some error. To implement in further OS implementation

    // Initialize port if it's not
    if(((RCC->AHB1ENR >> port_index) & 0x01) != 0x1)
        initialize_gpio_port(pin);
    

    // Reset MODE register. This makes pin mode INPUT as MODE register is set to 0b00 (input mode as per docs)
    gpio->MODER &= ~(0x3 << (pin_number * 2));

    if(mode == OUTPUT) {
        gpio->MODER |= (0x1 << (pin_number * 2)); // Set "general purpose output mode" (0b01, 0x1)

        // Set GPIO Output Type register, bits [31:16] RESERVED, set OTYPER to push-pull (0x0)
        gpio->OTYPER &= ~(0x1 << pin_number);

        // Not really needed, but set GPIO OSPEED register to medium speed (0b01)
        gpio->OSPEEDR &= ~(0x3 << (pin_number * 2));
        gpio->OSPEEDR |= (0x1 << (pin_number * 2));
    }
}


void gpio_write(GPIO_Pin_t pin, GPIO_DState_t state) {
    uint8_t port_index = get_port_index(pin);
    uint8_t pin_number = get_abstracted_pin(pin);
    GPIO_Typedef* gpio = get_abstracted_port(pin);

    if(!gpio) return; // TODO: use PC13 (in-built LED) to signal some error. To implement in further OS implementation

    // Initialize port if it's not
    if(((RCC->AHB1ENR >> port_index) & 0x01) != 0x1)
        initialize_gpio_port(pin);
    
    if(state == 0) gpio->BSRR = (1 << (pin_number + 16));
    else gpio->BSRR = (1 << pin_number);
}

uint8_t gpio_read(GPIO_Pin_t pin) {
    uint8_t port_index = get_port_index(pin);
    uint8_t pin_number = get_abstracted_pin(pin);
    GPIO_Typedef* gpio = get_abstracted_port(pin);

    if(!gpio) return 0xFF; // TODO: use PC13 (in-built LED) to signal some error. To implement in further OS implementation

    // Initialize port if it's not
    if(((RCC->AHB1ENR >> port_index) & 0x01) != 0x1)
        initialize_gpio_port(pin);
    
    return ((gpio->IDR >> pin_number) & 0x1);
}


void digitalWrite(GPIO_Pin_t pin, GPIO_DState_t state) {
    gpio_write(pin, state);
}

GPIO_DState_t digitalRead(GPIO_Pin_t pin) {
    return (GPIO_DState_t)gpio_read(pin);
}

void analogWrite(GPIO_Pin_t pin, uint8_t value);
uint8_t analogRead(GPIO_Pin_t pin);