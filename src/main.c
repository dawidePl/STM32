#include "Drivers/SYSCLK/sysclk.h"
#include "Generic/generic.h"
#include "Drivers/GPIO/GPIO.h"

int main() {
    gpio_set(GPIOC_en, 13, 0);

    while(1);
}