#ifndef TUSB_CONFIG_H_
#define TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUSB_MCU       OPT_MCU_STM32F4
#define CFG_TUSB_OS        OPT_OS_NONE
#define CFG_TUSB_DEBUG     0

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))

/* device-only, port 0, full speed (STM32F411's OTG_FS has no HS PHY).
 * This is what actually makes tusb_init() call tud_init(): without it,
 * TUD_OPT_RHPORT is never defined and tusb_init() silently no-ops. */
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

#define CFG_TUD_ENDPOINT0_SIZE 64

#define CFG_TUD_CDC 1

#define CFG_TUD_CDC_RX_BUFSIZE 512
#define CFG_TUD_CDC_TX_BUFSIZE 512
#define CFG_TUD_CDC_EP_BUFSIZE 64

#ifdef __cplusplus
}
#endif

#endif
