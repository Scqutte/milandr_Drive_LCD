#include "system/tick.h"

#include "config.h"
#include "drivers/sound.h"
#include "MDR32Fx.h"
#include "system_MDR32F9Qx.h"

#define TICK_MS_PER_SECOND 1000U
#define TICK_ISR_HZ 10000U
#define TICK_US_PER_ISR 100U
#define TICK_ISR_PER_MS (TICK_MS_PER_SECOND / TICK_US_PER_ISR)
#define FRAME_PERIOD_MS (TICK_MS_PER_SECOND / GAME_TARGET_FPS)

static volatile uint32_t tick_ms;
static volatile uint8_t tick_sub_ms;
static uint32_t next_frame_ms;

void SysTick_Handler(void)
{
    sound_tick_100us();

    tick_sub_ms++;
    if (tick_sub_ms >= TICK_ISR_PER_MS) {
        tick_sub_ms = 0U;
        tick_ms++;
    }
}

void tick_init(void)
{
    tick_ms = 0;
    tick_sub_ms = 0U;
    next_frame_ms = 0;
    SystemCoreClockUpdate();
    (void)SysTick_Config(SystemCoreClock / TICK_ISR_HZ);
}

uint32_t tick_get_ms(void)
{
    return tick_ms;
}

uint32_t tick_get_us(void)
{
    uint32_t ms_before;
    uint32_t ms_after;
    uint32_t ticks_left;
    uint32_t ticks_per_isr;
    uint32_t ticks_per_us;
    uint32_t elapsed_us;
    uint8_t sub_ms_before;
    uint8_t sub_ms_after;

    do {
        ms_before = tick_ms;
        sub_ms_before = tick_sub_ms;
        ticks_left = SysTick->VAL;
        ms_after = tick_ms;
        sub_ms_after = tick_sub_ms;
    } while ((ms_before != ms_after) || (sub_ms_before != sub_ms_after));

    ticks_per_isr = SysTick->LOAD + 1U;
    ticks_per_us = SystemCoreClock / 1000000U;

    if (ticks_per_us == 0U) {
        return ms_before * 1000U;
    }

    elapsed_us = (ticks_per_isr - ticks_left) / ticks_per_us;
    if (elapsed_us >= TICK_US_PER_ISR) {
        elapsed_us = TICK_US_PER_ISR - 1U;
    }

    return (ms_before * 1000U) + ((uint32_t)sub_ms_before * TICK_US_PER_ISR) + elapsed_us;
}

void tick_delay_ms(uint32_t delay_ms)
{
    uint32_t start = tick_get_ms();

    while ((tick_get_ms() - start) < delay_ms) {
    }
}

uint8_t tick_should_run_frame(void)
{
    uint32_t now = tick_get_ms();

    if ((int32_t)(now - next_frame_ms) < 0) {
        return 0;
    }

    next_frame_ms = now + FRAME_PERIOD_MS;
    return 1;
}
