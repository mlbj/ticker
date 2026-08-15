#include "stm32f4xx.h"
#include "osc.h"
#include "tusb.h"
#include "pulse.h"

static void usb_hw_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* PA11/PA12 as AF10 (USB_OTG_FS_DM/DP), very high speed, no pull,
     * push-pull output type */
    GPIOA->MODER = (GPIOA->MODER & ~(3U << (11 * 2) | 3U << (12 * 2))) |
                   (2U << (11 * 2)) | (2U << (12 * 2));
    GPIOA->OSPEEDR |= (3U << (11 * 2)) | (3U << (12 * 2));
    GPIOA->PUPDR &= ~(3U << (11 * 2) | 3U << (12 * 2));
    GPIOA->OTYPER &= ~((1U << 11) | (1U << 12));
    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(0xFU << ((11 - 8) * 4) | 0xFU << ((12 - 8) * 4))) |
                    (10U << ((11 - 8) * 4)) | (10U << ((12 - 8) * 4));

    RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;

    /* This board doesn't reliably wire PA9 to VBUS, so tell the peripheral
     * to skip VBUS sensing and assume a session is always present:
     * matches TinyUSB's own stm32f411blackpill board support. */
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    USB_OTG_FS->GCCFG &= ~(USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_VBUSASEN);

    NVIC_SetPriority(OTG_FS_IRQn, 0);
    NVIC_EnableIRQ(OTG_FS_IRQn);
}

int main(void) {
    osc_config();
    usb_hw_init();
    pulse_timer_init();
    tusb_init();

    while (1) {
        tud_task();

        if (tud_cdc_available()) {
            uint8_t buf[64];
            uint32_t n = tud_cdc_read(buf, sizeof(buf));
            tud_cdc_write(buf, n);
            tud_cdc_write_flush();
        }
    }
}

void error_handler(void) {
    __disable_irq();
    while (1) {
    }
}

