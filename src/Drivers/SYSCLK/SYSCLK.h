#pragma once

#include "Generic/generic.h"

#define RCC_BASE 0x40023800UL

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t RESERVED0;
    volatile uint32_t RESERVED1;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t RESERVED2;
    volatile uint32_t RESERVED3;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t RESERVED4;
    volatile uint32_t RESERVED5;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t RESERVED6;
    volatile uint32_t RESERVED7;
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    volatile uint32_t RESERVED8;
    volatile uint32_t RESERVED9;
    volatile uint32_t APB1LPENR;
    volatile uint32_t APB2LPENR;
    volatile uint32_t RESERVED10;
    volatile uint32_t RESERVED11;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t RESERVED12;
    volatile uint32_t RESERVED13;
    volatile uint32_t SSCGR;
    volatile uint32_t PLLI2SCFGR;
    volatile uint32_t RESERVED14;
    volatile uint32_t DCKCFGR;
} RCC_Typedef;

#define RCC ((RCC_Typedef *) RCC_BASE)

#define HSI_FREQ 16000000UL
#define HSE_FREQ 25000000UL
#define LSE_FREQ 32768UL

#define TIM2_BASE 0x40000000UL
#define TIM3_BASE 0x40000400UL
#define TIM4_BASE 0x40000800UL
#define TIM5_BASE 0x40000C00UL

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR; // CHECK PAGE 368, DIFFERS FOR TIM_2 AND TIM_5
} TIM_Typedef;


#define TIM2 ((TIM_Typedef *) TIM2_BASE)
#define TIM3 ((TIM_Typedef *) TIM3_BASE)
#define TIM4 ((TIM_Typedef *) TIM4_BASE)
#define TIM5 ((TIM_Typedef *) TIM5_BASE)


void initialize_pll_clk();
uint32_t get_sys_clk_freq();

void delay(uint32_t time);
void enable_gpio_clock(uint8_t port_index);