#include "stm32f4xx.h"
#include "tusb.h"

/* Just needs to exist and return: with no HAL, the weak default vector
 * would otherwise spin forever on the first 1ms tick from osc_config(). */
void SysTick_Handler(void) {
}

void OTG_FS_IRQHandler(void) {
    tud_int_handler(0);
}
