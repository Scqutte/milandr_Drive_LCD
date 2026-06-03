#include "system/tick.h"

#include "config.h"
#include "MDR32Fx.h"
#include "system_MDR32F9Qx.h"

#define TICK_MS_PER_SECOND 1000U
#define FRAME_PERIOD_MS (TICK_MS_PER_SECOND / GAME_TARGET_FPS)

static volatile uint32_t tick_ms;
static uint32_t next_frame_ms;

void SysTick_Handler(void)
{
    tick_ms++;
}

void tick_init(void)
{
    tick_ms = 0;
    next_frame_ms = 0;
    SystemCoreClockUpdate();
    (void)SysTick_Config(SystemCoreClock / TICK_MS_PER_SECOND);
}

uint32_t tick_get_ms(void)
{
    return tick_ms;
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
