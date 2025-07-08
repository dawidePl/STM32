#include "Drivers/SYSCLK/sysclk.h"

/*
    STM32 reference, https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
    Section 6 - Reset and clock control (RCC),
*/

/*
    Getting system clock frequency used at a time.

    @return Returns clock frequency in MHz.
*/
uint32_t get_sys_clk_freq() {
    uint32_t sysclk = 0;
    uint32_t cfgr = RCC_CFGR; // RCC->CFGR, read the register
    uint8_t sws = (cfgr >> 2) & 0x3;

    uint32_t pllcfgr, pllsrc, pllm, plln, pllp_bits, pllp, pll_input_freq;

    switch(sws) {
        // HSI used
        case 0x0:
            sysclk = HSI_FREQ;
            break;
        
        // HSE used
        case 0x1:
            sysclk = HSE_FREQ;
            break;

        // PLL used
        case 0x2:
            // PLL output frequency = (PLL input frequency / PLLM) * PLLN / PLLP
            pllcfgr = RCC_PLLCFGR;

            pllsrc = (pllcfgr >> 22) & 0x1; // Bit 22
            pllm = pllcfgr & 0x3F; // Bits 5:0
            plln = (pllcfgr >> 6) & 0x1FF; // Bits 14:6
            pllp_bits = (pllcfgr >> 16) & 0x3; // Bits 17:16

            pllp = (pllp_bits + 1) * 2;

            pll_input_freq = (pllsrc == 0) ? HSI_FREQ : HSE_FREQ;

            sysclk = (pll_input_freq / pllm) * plln / pllp;

            break;

        // Other cases
        default:
            break;
    }

    uint32_t hpre = (cfgr >> 4) & 0xF;
    const uint16_t HPRE_table[16] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        2, 4, 8, 16, 64, 128, 256, 512
    };

    sysclk /= HPRE_table[hpre];

    return sysclk;
}


/*
    Delay function, meant to stop the process for given amount of time.

    STM32F used is a 100 MHz MCU meaning 1 clock cycle takes 10 ns. 1 "NOP" instruction takes 1 clock cycle.
    10 ns = 0.00001 ms (1 * 10^(-5)).

    @param uint32_t time Delay time in ms.
    @returns void No return value.
*/
void delay(uint32_t time) {
    if(time == 0) return;

    uint32_t sysclk_freq = get_sys_clk_freq();
    uint32_t cycles_per_ms = sysclk_freq / 1000;

    while(time--) {
        uint32_t num_nop = cycles_per_ms;

        while(num_nop--)
            __asm("nop");
    }
}

/*
    Enables clock for given GPIO bus. For ease of use a bus can be selected as a number, which then becomes a bitshift offset as per AHB1ENR register memory map.

    @param GPIO_Bus_t bus Goes as letters from A to D where bus A is 0, increment by one for each bus.
*/
void enable_gpio_clock(GPIO_Bus_t bus) {
    if(bus > GPIOD_en) return; // Handle incorrect bus selection

    RCC_AHB1ENR |= (0x1 << bus);
}