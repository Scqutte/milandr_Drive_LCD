#include "drivers/sound.h"

#include "config.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32Fx.h"

#define SOUND_DAC_PORT MDR_PORTE
#define SOUND_DAC1_PIN_MASK (1U << 9)
#define SOUND_DAC2_PIN_MASK (1U << 0)
#define SOUND_DAC_PIN_MASK (SOUND_DAC1_PIN_MASK | SOUND_DAC2_PIN_MASK)
#define SOUND_DAC1_FUNC_MASK (3U << 18)
#define SOUND_DAC2_FUNC_MASK (3U << 0)
#define SOUND_DAC_FUNC_MASK (SOUND_DAC1_FUNC_MASK | SOUND_DAC2_FUNC_MASK)
#define SOUND_DAC1_PWR_MASK (3U << 18)
#define SOUND_DAC2_PWR_MASK (3U << 0)
#define SOUND_DAC_PWR_MASK (SOUND_DAC1_PWR_MASK | SOUND_DAC2_PWR_MASK)
#define SOUND_DAC1_PULL_MASK ((1U << 9) | (1U << 25))
#define SOUND_DAC2_PULL_MASK ((1U << 0) | (1U << 16))
#define SOUND_DAC_PULL_MASK (SOUND_DAC1_PULL_MASK | SOUND_DAC2_PULL_MASK)
#define SOUND_DAC1_ENABLE (1U << 2)
#define SOUND_DAC2_ENABLE (1U << 3)
#define SOUND_DAC_ENABLE SOUND_DAC2_ENABLE
#define SOUND_DAC_DATA_MASK 0x0FFFU
#define SOUND_PARTY_AMPLITUDE 2047U

static uint8_t test_route;

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

    if (test_route == 1U) {
        MDR_DAC->DAC2_DATA = ((uint32_t)value & SOUND_DAC_DATA_MASK);
    } else if (test_route == 2U) {
        MDR_DAC->DAC1_DATA = ((uint32_t)value & SOUND_DAC_DATA_MASK);
        MDR_DAC->DAC2_DATA = ((uint32_t)value & SOUND_DAC_DATA_MASK);
    } else {
        MDR_DAC->DAC1_DATA = ((uint32_t)value & SOUND_DAC_DATA_MASK);
    }
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
    SOUND_DAC_PORT->ANALOG &= ~SOUND_DAC_PIN_MASK;
    SOUND_DAC_PORT->FUNC &= ~SOUND_DAC_FUNC_MASK;
    SOUND_DAC_PORT->OE &= ~SOUND_DAC_PIN_MASK;
    SOUND_DAC_PORT->PWR &= ~SOUND_DAC_PWR_MASK;
    SOUND_DAC_PORT->PULL &= ~SOUND_DAC_PULL_MASK;
}

void sound_init(void)
{
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE | RST_CLK_PCLK_DAC, ENABLE);

    sound_init_pins();
    /* The board audio path was verified on DAC2/PE0. */
    MDR_DAC->CFG = SOUND_DAC_ENABLE;
    test_route = 1U;
    sound_write(SOUND_DAC_CENTER);
}

static void sound_play_tone_with_amplitude(uint16_t half_period_delay, uint16_t cycles, uint16_t amplitude)
{
    uint16_t i;
    uint16_t high = SOUND_DAC_CENTER + amplitude;
    uint16_t low = SOUND_DAC_CENTER - amplitude;

    for (i = 0; i < cycles; i++) {
        sound_write(high);
        sound_delay(half_period_delay);
        sound_write(low);
        sound_delay(half_period_delay);
    }

    sound_write(SOUND_DAC_CENTER);
}

void sound_play_shot(void)
{
    sound_play_tone(700, 70);
}

void sound_play_explosion(void)
{
    sound_play_tone(1400, 45);
    sound_play_tone(950, 45);
}

void sound_play_win(void)
{
    sound_play_tone(1100, 55);
    sound_play_tone(850, 55);
    sound_play_tone(650, 65);
}

void sound_set_test_route(uint8_t route)
{
    (void)route;
    test_route = 1U;
    sound_write(SOUND_DAC_CENTER);
}

void sound_play_test_party(void)
{
    sound_play_tone_with_amplitude(520, 900, SOUND_PARTY_AMPLITUDE);
    sound_play_tone_with_amplitude(420, 900, SOUND_PARTY_AMPLITUDE);
}

void sound_update(void)
{
}
