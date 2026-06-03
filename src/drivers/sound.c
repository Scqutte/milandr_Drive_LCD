#include "drivers/sound.h"

#include "config.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define SOUND_DAC_PORT MDR_PORTE
#define SOUND_DAC_PINS (PORT_Pin_0 | PORT_Pin_9)

static uint16_t sound_limit(uint16_t value)
{
    if (value < SOUND_DAC_MIN) {
        return SOUND_DAC_MIN;
    }

    if (value > SOUND_DAC_MAX) {
        return SOUND_DAC_MAX;
    }

    return value;
}

static void sound_delay(volatile uint16_t value)
{
    while (value--) {
    }
}

static void sound_write(uint16_t value)
{
    value = sound_limit(value);

    DAC1_SetData(value);
    DAC2_SetData(value);
}

static void sound_play_tone(uint16_t half_period_delay, uint16_t cycles)
{
    uint16_t i;
    uint16_t high = SOUND_DAC_CENTER + SOUND_DAC_AMPLITUDE;
    uint16_t low = SOUND_DAC_CENTER - SOUND_DAC_AMPLITUDE;

    for (i = 0; i < cycles; i++) {
        sound_write(high);
        sound_delay(half_period_delay);
        sound_write(low);
        sound_delay(half_period_delay);
    }

    sound_write(SOUND_DAC_CENTER);
}

static void sound_init_pins(void)
{
    PORT_InitTypeDef init;

    init.PORT_Pin = SOUND_DAC_PINS;
    init.PORT_OE = PORT_OE_IN;
    init.PORT_PULL_UP = PORT_PULL_UP_OFF;
    init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    init.PORT_PD = PORT_PD_DRIVER;
    init.PORT_GFEN = PORT_GFEN_OFF;
    init.PORT_FUNC = PORT_FUNC_PORT;
    init.PORT_SPEED = PORT_OUTPUT_OFF;
    init.PORT_MODE = PORT_MODE_ANALOG;

    PORT_Init(SOUND_DAC_PORT, &init);
}

void sound_init(void)
{
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE | RST_CLK_PCLK_DAC, ENABLE);

    sound_init_pins();
    /* Independent DAC mode, AVCC reference for both channels. */
    DAC_Init(0U, 0U, 0U);
    DAC1_Cmd(ENABLE);
    DAC2_Cmd(ENABLE);
    sound_write(SOUND_DAC_CENTER);
}

void sound_play_shot(void)
{
    sound_play_tone(55, 18);
}

void sound_play_explosion(void)
{
    sound_play_tone(130, 12);
    sound_play_tone(95, 10);
}

void sound_play_win(void)
{
    sound_play_tone(100, 18);
    sound_play_tone(75, 18);
    sound_play_tone(55, 18);
}

void sound_update(void)
{
}
