#include "pulse.h"
#include "stm32f4xx.h"

#define PULSE_TIMER_CLK_HZ 96000000UL

void pulse_timer_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    /* Set PA8 as AF1 (TIM1_CH1).
     *
     * The first statement configures PA8 to be controlled by an alternate
     * function. The second selects AF1 for PA8, which corresponds to TIM1_CH1
     * on the STM32F411.
     *
     * GPIOA->MODER is a 32-bit register with two bits per GPIO pin:
     *
     *   31                      0
     *   +----+----+-----+---+---+
     *   |PA15|PA14| ... |PA1|PA0|
     *   +----+----+-----+---+---+
     *     2b   2b         2b  2b
     *
     * Each pin can be configured as:
     *
     *   | bits | mode                    |
     *   |------|-------------------------|
     *   | 00   | Input                   |
     *   | 01   | General-purpose output  |
     *   | 10   | Alternate function      |
     *   | 11   | Analog                  |
     *
     * PA8 corresponds to bits 16 and 17, since 8 * 2 = 16.
     * The first statement clears those two bits and sets them to 10,
     * selecting alternate-function mode.
     *
     * GPIOA->AFR[1] contains the alternate-function selections for PA8-PA15.
     * Each pin has a 4-bit field. PA8 uses bits 3:0 of AFR[1].
     * The second statement clears those four bits and sets them to 0001,
     * selecting AF1, which maps PA8 to TIM1_CH1.
     */
    GPIOA->MODER = (GPIOA->MODER & ~(3U << (8 * 2))) | (2U << (8 * 2));
    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(0xFU << 0)) | (1U << 0);

    /* Configures TIM1 counter and channel 1. PSC is the prescaler, setting it 
     * to 0 means that timer clock = timer clock/ (PSC + 1) = timer clock, 
     * that is, no prescaling.
     *
     * ARR stands for Auto-Realod Register. It determines the maximum value 
     * the counter reaches before wrapping/ restarting. In this case 
     * 0xFFF = 65535, the maximum possible value.
     *
     * CCR1 is the Capture/Compare Register 1, so this sets the compare point to 0.
     */  
    TIM1->PSC = 0;
    TIM1->ARR = 0xFFFF;
    TIM1->CCR1 = 0;

    /* PWM mode 1, preloaded so freq/width updates apply glitch-free on
     * the next update event instead of mid-cycle. */
    
    /* CCMR1 is the Capture/ Compare mode register 1. For channel 1, the important field
     * is OCM1, Output Compare 1 Mode. On the F411, it's a 3-bit field
     *
     *   | bits (6:4) | mode              |
     *   |------------|-------------------|
     *   | 000        | Frozen            |
     *   | 001        | Active on match   |
     *   | 010        | Inactive on match |
     *   | 011        | toggle            | 
     *   | 100        | Force inactive    |
     *   | 101        | Force active      |
     *   | 110        | PWM mode 1        |
     *   | 111        | PWM mode 2        |
     *
     * We want 110 (PWM mode 1), and CMSIS header conveniently gives us
     *
     * TIM_CCMR1_0C1M_0 (bit 0) = 0
     * TIM_CCMR1_OC1M_1 (bit 1) = 1
     * TIM_CCMR1_OC1M_2 (bit 2) = 1
     *
     * So this clears those three bits and sets them as 110.
     */
    TIM1->CCMR1 = (TIM1->CCMR1 & ~TIM_CCMR1_OC1M) | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

    /* Enable preload for CCR1 and ARR. The first statement enables preload for
     * CCR1.  Without preload we have
     *
     *    CPU writes CCR1
     *         ->
     *    CCR1 changes immediately
     *         ->
     *    potentially changes current pulse
     *
     * With preload
     *
     *    CPU writes CCR1
     *         ->
     *    preload/shadow register
     *         ->(next update event)
     *    active CCR1
     *         ->
     *    new pulse parameters,
     *
     * So if we change CCR1 while the timer is running (which is very likely), the
     * new value can take effect cleanly at the next update event rather than halfway
     * through the current PWM cycle.
     *
     * The second statement enables ARR preload.
     * ARPE = Auto-Reload Preload Enable. Without preload  we have
     *
     *  CPU -> ARR -> Timer immediately sees the new period
     *
     * With preload
     *
     *  CPU -> ARR preload
     *            -> (update event)
     *         Active ARR
     *
     * So
     *   CCR1 preload: duty-cycle changes happen cleanly
     *   ARR preload: period/frequency changes happen cleanly
     */
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM1->CR1 |= TIM_CR1_ARPE;

    // Enable TIM1 channel 1 output
    TIM1->CCER |= TIM_CCER_CC1E;
    
    // EGR is the Event Generation Register. UG means Update Generation
    TIM1->EGR |= TIM_EGR_UG;

    // CR1 is the control register. CEN means Counter ENable.
    TIM1->CR1 |= TIM_CR1_CEN;
}

bool pulse_set_frequency(double freq_hz) {
    if (freq_hz <= 0.0) {
        return false;
    }

    uint32_t arr = (uint32_t)(PULSE_TIMER_CLK_HZ / freq_hz) - 1U;
    if (arr < 1U || arr > 0xFFFFU) {
        return false;
    }

    TIM1->ARR = arr;
    if (TIM1->CCR1 > arr) {
        TIM1->CCR1 = arr;
    }
    TIM1->EGR |= TIM_EGR_UG;

    return true;
}

bool pulse_set_width(double width_s) {
    if (width_s < 0.0) {
        return false;
    }

    uint32_t ccr1 = (uint32_t)(width_s * PULSE_TIMER_CLK_HZ);
    if (ccr1 > TIM1->ARR) {
        return false;
    }

    TIM1->CCR1 = ccr1;
    TIM1->EGR |= TIM_EGR_UG;

    return true;
}

void pulse_set_output(bool enable) {
    if (enable) {
        TIM1->BDTR |= TIM_BDTR_MOE;
    } else {
        TIM1->BDTR &= ~TIM_BDTR_MOE;
    }
}
