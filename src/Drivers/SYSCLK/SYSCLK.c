#include "Drivers/SYSCLK/sysclk.h"
#include "Drivers/GPIO/GPIO.h"
#include "Drivers/Memory/FLASH.h"

/*
    STM32 reference, https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
    Section 6 - Reset and clock control (RCC),
*/

/*
    Initialize PLL clock, use HSI for input, set on 84 MHz.
*/
void initialize_pll_clk() {
    // Turn on HSI
    RCC->CR |= 0x1;
    while(!(RCC->CR & (1 << 1))); // wait for HSI

    // Switch to HSI
    RCC->CFGR &= ~0x3;
    RCC->CFGR |= 0x0; // HSI selected
    while(((RCC->CFGR >> 2) & 0x3) != 0); // Wait for the switch to happen

    RCC->CR &= ~(0x1 << 24); // PLL off
    while(RCC->CR & (0x1 << 25)); // Wait for PLLRDY = 0

    RCC->CFGR &= ~(0b1111 << 4); // HPRE set to 1
    RCC->CFGR &= ~(0b111 << 10); // clear PPRE1
    RCC->CFGR |= (0b100 << 10); // PPRE1 set to 2

    // PLLM = 16 (bits 5:0)
    // PLLN = 168 (bits 14:6)
    // PLLP = 2 (encoded as 0b00 at bits 17:16)
    // PLLSRC = 0 (HSI) bit 22
    // PLLQ = 4 (bits 27:24)
    uint32_t pllm = 16;
    uint32_t plln = 168;
    uint32_t pllp = 0; // 00b for divide by 2
    uint32_t pllsrc = 0; // HSI
    uint32_t pllq = 4;

    RCC->PLLCFGR = 0;

    RCC->PLLCFGR = (pllm & 0x3F)         // PLLM bits 5:0
                | ((plln & 0x1FF) << 6) // PLLN bits 14:6
                | (pllp << 16)          // PLLP bits 17:16
                | (pllsrc << 22)        // PLLSRC bit 22
                | ((pllq & 0xF) << 24); // PLLQ bits 27:24

    // Enable PLL
    RCC->CR |= (0x1 << 24); // PLL on

    uint32_t timeout = 1000000;
    while((RCC->CR & (0x1 << 25)) == 0 && --timeout); // wait until PLLRDY = 1

    // Set PLL as SYSCLK, so 0b10 in SW bits
    RCC->CFGR &= ~0x3; // clear SW
    RCC->CFGR |= 0b10; // PLL Selected
    
    // Wait until PLL used as SYSCLK (SWS bits 3:2 = 10)
    while ((RCC->CFGR & 0xC) != 0x8);


}



/*
    Getting system clock frequency used at a time.

    @return Returns clock frequency in Hz.
*/
uint32_t get_sys_clk_freq() {
    uint32_t sysclk = 0;
    uint32_t cfgr = RCC->CFGR; // RCC->CFGR
    uint8_t sws = (cfgr >> 2) & 0x3;

    uint32_t pllcfgr, pllsrc, pllm, plln, pllp_bits, pllp;
    uint64_t pll_vco, pll_output;
    uint32_t pll_input_freq;

    switch(sws) {
        case 0x0: // HSI
            sysclk = HSI_FREQ;
            break;

        case 0x1: // HSE
            sysclk = HSE_FREQ;
            break;

        case 0x2: // PLL
            pllcfgr = RCC->PLLCFGR;
            pllsrc = (pllcfgr >> 22) & 0x1;
            pllm = pllcfgr & 0x3F;
            plln = (pllcfgr >> 6) & 0x1FF;
            pllp_bits = (pllcfgr >> 16) & 0x3;

            if(pllm == 0) return 0; // avoid division by zero

            switch(pllp_bits) {
                case 0: pllp = 2; break;
                case 1: pllp = 4; break;
                case 2: pllp = 6; break;
                case 3: pllp = 8; break;
                default: pllp = 2; break;
            }

            pll_input_freq = (pllsrc == 0) ? HSI_FREQ : HSE_FREQ;

            pll_vco = ((uint64_t)pll_input_freq / pllm) * plln;
            pll_output = pll_vco / pllp;

            sysclk = (uint32_t)pll_output;
            break;

        default:
            sysclk = 0; // unknown clock source
            break;
    }

    uint32_t hpre = (cfgr >> 4) & 0xF;
    const uint16_t HPRE_table[16] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        2, 4, 8, 16, 64, 128, 256, 512
    };

    if(hpre < 16)
        sysclk /= HPRE_table[hpre];

    return sysclk;
}


// Delay helper functions
// void delay_short(uint32_t time, TIM_Typedef tim);
// void delay_long(uint32_t time, TIM_Typedef tim);

TIM_Typedef* get_available_tim();

void wait(uint32_t time, TIM_Typedef* tim);

/*
    Stops code execution for <time> ammount of milliseconds. Uses TIMx hardware clock to count the time. Uses either TIM3 or TIM5 counter.

    TODO: for whatever reason delay is 4 times too fast.
    TODO: turn off TIMx RCC clock after delay ends, not after each wait() call.
    TODO: if get_available_tim() returns (void*)0, force-use TIM5.

    @param uint32_t time Time in milliseconds.
*/
void delay(uint32_t time) {
    if(time == 0) return;

    TIM_Typedef* tim = get_available_tim();
    uint8_t tim_num = tim == TIM2 ? 2 : 5;

    // Turn on RCC clock for TIM2/5
    RCC->APB1ENR |= (tim_num == 2) ? (0x1) : (0x1 << 3);

    uint32_t max_time = 4000000;
    uint32_t loop_ammount = (uint32_t)((time / max_time) + 1);

    while(loop_ammount--) {
        wait(time, tim);
        time -= max_time;
    }

    // Turn off RCC clock for TIM2/5
    RCC->APB1ENR &= (tim_num == 2) ? ~(0x1) : ~(0x1 << 3);
}

/*
    Get either TIM2 or TIM5, depending on which one is available to use.
*/
TIM_Typedef* get_available_tim() {
    // Check by RCC bus, TIM with disabled RCC bus is free to use
    if((RCC->APB1ENR & 0x1) == 0) return TIM2;
    if((RCC->APB1ENR & 0b1000) == 0) return TIM5;

    // Both busses are on, check by TIMx CR register
    if((TIM2->CR1 & 0x1) == 0) return TIM2; // TIM2 CR1 CEN bit is set to 0 meaning it's not in use
    if((TIM5->CR1 & 0x1) == 0) return TIM5; // TIM5 CR1 CEN bit is set to 0 meaning it's not in use

    // TIM2 is in use, so is TIM5. For now - force TIM5 to be used
    return TIM5;
}

/*
    Delay helper function. Sets up TIMx timer for 10000 Hz (1KHz) frequency.

    @param uint32_t time Delay duration in ms.
    @param TIM_Typedef* tim Tim that will be used.
*/
void wait(uint32_t time, TIM_Typedef* tim) {
    uint32_t sysclk_freq = get_sys_clk_freq();
    uint32_t cfgr = RCC->CFGR;

    uint32_t ppre1_bits = (cfgr >> 10) & 0x7;
    uint32_t apb1_prescaler = (ppre1_bits < 4) ? 1 : (1 << (ppre1_bits - 3));

    uint32_t pclk1 = sysclk_freq / apb1_prescaler;
    uint32_t tim_clk = (apb1_prescaler == 1) ? pclk1 : (pclk1 * 2);

    uint32_t desired_tick = 10000; // 10 kHz = 0.1ms per tick
    uint32_t prescaler = (tim_clk / desired_tick) - 1;

    tim->CR1 &= ~(0x1); // Disable TIM3 clock, optional

    tim->PSC = prescaler;
    tim->CNT = 0; // reset count, just in case
    tim->ARR = (time * 10) - 1; // Overflow just when time passed (time is multiplied by 10 because tick rate is at 10 KHz, so we need to adjust for that!)
    tim->EGR |= 0x1; // Generate update event - load ARR and PSC, otherwise they stay in a buffer and are not loaded!

    tim->CR1 |= 0x1; // Enable clock
    tim->SR &= ~0x1; // reset UIF flag

    while((tim->SR & 0x1) == 0); // Wait for UIF flag to be set
    tim->CR1 &= ~0x1; // Stop the counter
}


/*
    Enables clock for given GPIO bus. For ease of use a bus can be selected as a number, which then becomes a bitshift offset as per AHB1ENR register memory map.

    @param GPIO_Bus_t bus Goes as letters from A to D where bus A is 0, increment by one for each bus.
*/
void enable_gpio_clock(GPIO_Bus_t bus) {
    if(bus > GPIOD_en) return; // Handle incorrect bus selection

    RCC->AHB1ENR |= (0x1 << bus);
}