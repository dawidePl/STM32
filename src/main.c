#include "Generic/generic.h"
#include "Drivers/GPIO/GPIO.h"
#include "Drivers/SYSCLK/SYSCLK.h"

int main() {
    initialize_pll_clk();

    while(1) {
        gpio_set(GPIOC_en, 13, 0);
        delay(1000);

        gpio_set(GPIOC_en, 13, 1);
        delay(1000);
    }
}