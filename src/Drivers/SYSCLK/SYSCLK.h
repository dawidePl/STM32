#pragma once

#include "Generic/generic.h"

#define RCC_BASE 0x40023800UL
#define RCC_PLLCFGR (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define HSI_FREQ 16000000UL
#define HSE_FREQ 25000000UL
#define LSE_FREQ 32768UL

#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30))

uint32_t get_sys_clk_freq();

void delay(uint32_t time);

void enable_gpio_clock(GPIO_Bus_t bus);