#include "osc.h"
#include "stm32f4xx.h"

extern void error_handler(void);


volatile uint32_t osc_get_sys_clock_freq(void) {
    uint32_t pllm = 0U;
    uint32_t pllvco = 0U;
    uint32_t pllp = 0U;
    uint32_t sysclockfreq = 0U;

    // Get SYSCLK source
    switch (RCC->CFGR & RCC_CFGR_SWS) {
    // HSI used as a system clock source
    case RCC_CFGR_SWS_HSI:
        sysclockfreq = HSI_VALUE;
        break;

    // HSE used as system clock  source
    case RCC_CFGR_SWS_HSE:
        sysclockfreq = HSE_VALUE;
        break;

    // PLL used as system clock  source
    case RCC_CFGR_SWS_PLL:
        // PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
        // SYSCLK = PLL_VCO / PLLP
        pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
        if ((uint32_t)(RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) != RCC_PLLSOURCE_HSI) {
            // HSE used as PLL clock source
            pllvco = (uint32_t)((((uint64_t)HSE_VALUE * ((uint64_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos)))) / (uint64_t)pllm);
        } else {
            // HSI used as PLL clock source
            pllvco = (uint32_t)((((uint64_t)HSI_VALUE * ((uint64_t)((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos)))) / (uint64_t)pllm);
        }
        pllp = ((((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos) + 1U) * 2U);

        sysclockfreq = pllvco / pllp;
        break;

    default:
        sysclockfreq = HSI_VALUE;
        break;
    }
    return sysclockfreq;
}

void osc_config(void) {
    volatile uint32_t tmp = 0x00U;
    uint32_t PLLM = 25;
    uint32_t PLLN = 192;
    // RCC_PLLP_DIV2;
    uint32_t PLLP = 2;
    uint32_t PLLQ = 4;

    // SysTick_Config(SystemCoreClock / (1000U / 1U));

    // 1. Configure the main internal regulator output voltage.
    // Delay after an RCC peripheral clock enabling
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    tmp = RCC->APB1ENR & RCC_APB1ENR_PWREN;
    (void)tmp;

    // 2. (Optional) Set voltage scale to Scale1 for maximum performance
    // Delay after an RCC peripheral clock enabling
    PWR->CR = (PWR->CR & ~PWR_CR_VOS) | PWR_REGULATOR_VOLTAGE_SCALE1;
    tmp = PWR->CR & PWR_CR_VOS;
    (void)tmp;

    // 3. Configure Flash prefetch, Instruction cache, Data cache.
    // Latency must go up before SYSCLK does: 3WS is required above 90MHz
    // (F411 datasheet, VDD 2.7-3.6V range) for the 96MHz target below.
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;

    // 4. Enable HSE and wait until it's ready, or bail out if it never locks
    RCC->CR |= RCC_CR_HSEON;
    uint32_t hse_timeout = HSE_STARTUP_TIMEOUT;
    while (((RCC->CR & RCC_CR_HSERDY) == 0) && (--hse_timeout != 0)) {}
    if (hse_timeout == 0) {
        error_handler();
    }

    // 5. Configure PLL. PLLM=25, PLLN=192, PLLP=2, PLLQ=4, PLLSRC=HSE
    // Disable the main PLL
    *(volatile uint32_t *)RCC_CR_PLLON_BB = DISABLE;

    // Wait till PLL is NOT? ready
    while ((RCC->CR & RCC_CR_PLLRDY) != 0) {}

    // Configure the main PLL clock source, multiplication and division factors.
    RCC->PLLCFGR = RCC_PLLSOURCE_HSE |
                   PLLM |
                   (PLLN << RCC_PLLCFGR_PLLN_Pos) |
                   (((PLLP >> 1U) - 1U) << RCC_PLLCFGR_PLLP_Pos) |
                   (PLLQ << RCC_PLLCFGR_PLLQ_Pos);

    // 6. Enable the main PLL, and wait for it to be ready
    *(volatile uint32_t *)RCC_CR_PLLON_BB = ENABLE;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}

    // Initializes the CPU, AHB and APB buses clocks

    // // Does not seem to be necessary
    // if (FLASH_LATENCY_0 > (FLASH->ACR & FLASH_ACR_LATENCY)){
    //   *(__IO uint8_t *)ACR_BYTE0_ADDRESS = (uint8_t)(FLASH_LATENCY_0);
    // }

    // HCLK Configuration
    // As the clock type is HCLK
    // Set the highest APBx dividers in order to ensure that we do not go through
    //      a non-spec phase whatever we decrease or increase HCLK
    // As the clock type is PCLK1
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_HCLK_DIV16;

    // As the clock type is PCLK2
    // Since clock type
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | (RCC_HCLK_DIV16 << 3);

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_SYSCLK_DIV1;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_SYSCLKSOURCE_PLLCLK;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    // PCLK1 Configuration (APB1 <= 50 MHz max, so /2 from a 96 MHz HCLK)
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_HCLK_DIV2;

    // PCLK2 Configuration
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | RCC_CFGR_PPRE2_DIV1;

    // Update the SystemCoreClock global variable
    SystemCoreClock = osc_get_sys_clock_freq() >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];

    // Configure the source of time base considering new system clocks settings
    // Configure the SysTick to have interrupt in 1ms time basis
    // 1U = 1 KHz
    SysTick_Config(SystemCoreClock / (1000U / 1U));
}

void osc_hse_debug(void) {
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configure PA8 as Alternate Function (AF0 = MCO1)
    GPIOA->MODER &= ~(3U << (8 * 2));
    GPIOA->MODER |= (2U << (8 * 2));

    // AF0 for PA8
    GPIOA->AFR[1] &= ~(0xFU << ((8 - 8) * 4));

    // 0U -> /1 -> 96 MHz
    // 4U -> /2 -> 48 MHz
    // 5U -> /3 -> 32 MHz
    // 6U -> /4 -> 24 MHz
    // 7U -> /5 -> 19.2 MHz
    uint32_t div_bits = (4U << 24);

    // Select PLL as MCO1 source (bits 22:21 = 11)
    // Select prescaler /2 (bits 26:24 = 100)
    // clear MCO1+PRE
    RCC->CFGR &= ~((3U << 21) | (7U << 24));
    RCC->CFGR |= (3U << 21) | div_bits;
}
