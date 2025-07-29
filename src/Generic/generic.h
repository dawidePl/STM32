#pragma once

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef enum {true, false} bool;

typedef enum {
    GPIOA_en = 0,
    GPIOB_en = 1,
    GPIOC_en = 2,
    GPIOD_en = 3
} GPIO_Port_t;

void register_write(uint32_t addr, uint32_t data);