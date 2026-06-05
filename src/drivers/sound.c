#include "drivers/sound.h"

#include "config.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32Fx.h"
#include "system/tick.h"

#define SOUND_DAC_PORT MDR_PORTE
#define SOUND_DAC2_PIN_MASK (1U << 0)
#define SOUND_DAC2_FUNC_MASK (3U << 0)
#define SOUND_DAC2_PWR_MASK (3U << 0)
#define SOUND_DAC2_PULL_MASK ((1U << 0) | (1U << 16))
#define SOUND_DAC2_ENABLE (1U << 3)
#define SOUND_DAC_ENABLE SOUND_DAC2_ENABLE
#define SOUND_DAC_DATA_MASK 0x0FFFU
#define SOUND_QUEUE_MAX 4U

typedef struct {
    uint16_t half_period_ms;
    uint16_t duration_ms;
} SoundTone;

static SoundTone sound_queue[SOUND_QUEUE_MAX];
static uint8_t sound_queue_count;
static uint8_t sound_queue_index;
static uint8_t sound_output_high;
static uint8_t sound_active;
static uint32_t sound_next_toggle_ms;
static uint32_t sound_tone_end_ms;

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

static void sound_write(uint16_t value)
{
    value = sound_limit(value);
    MDR_DAC->DAC2_DATA = ((uint32_t)value & SOUND_DAC_DATA_MASK);
}

static void sound_init_pins(void)
{
    SOUND_DAC_PORT->ANALOG &= ~SOUND_DAC2_PIN_MASK;
    SOUND_DAC_PORT->FUNC &= ~SOUND_DAC2_FUNC_MASK;
    SOUND_DAC_PORT->OE &= ~SOUND_DAC2_PIN_MASK;
    SOUND_DAC_PORT->PWR &= ~SOUND_DAC2_PWR_MASK;
    SOUND_DAC_PORT->PULL &= ~SOUND_DAC2_PULL_MASK;
}

void sound_init(void)
{
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE | RST_CLK_PCLK_DAC, ENABLE);

    sound_init_pins();
    /* The board audio path was verified on DAC2/PE0. */
    MDR_DAC->CFG = SOUND_DAC_ENABLE;
    sound_active = 0U;
    sound_queue_count = 0U;
    sound_queue_index = 0U;
    sound_output_high = 0U;
    sound_write(SOUND_DAC_CENTER);
}

static void sound_start_sequence(const SoundTone *tones, uint8_t count)
{
    uint16_t i;
    uint32_t now = tick_get_ms();

    if (count > SOUND_QUEUE_MAX) {
        count = SOUND_QUEUE_MAX;
    }

    if (count == 0U) {
        sound_active = 0U;
        sound_write(SOUND_DAC_CENTER);
        return;
    }

    for (i = 0U; i < count; i++) {
        sound_queue[i] = tones[i];
    }

    sound_queue_count = count;
    sound_queue_index = 0U;
    sound_output_high = 0U;
    sound_active = 1U;
    sound_next_toggle_ms = now;
    sound_tone_end_ms = now + sound_queue[0].duration_ms;
}

void sound_play_shot(void)
{
    static const SoundTone tones[] = {
        {1U, 55U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])));
}

void sound_play_explosion(void)
{
    static const SoundTone tones[] = {
        {3U, 90U},
        {2U, 90U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])));
}

void sound_play_win(void)
{
    static const SoundTone tones[] = {
        {3U, 90U},
        {2U, 90U},
        {1U, 120U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])));
}

void sound_update(void)
{
    uint32_t now;

    if (!sound_active) {
        return;
    }

    now = tick_get_ms();

    if ((int32_t)(now - sound_tone_end_ms) >= 0) {
        sound_queue_index++;

        if (sound_queue_index >= sound_queue_count) {
            sound_active = 0U;
            sound_write(SOUND_DAC_CENTER);
            return;
        }

        sound_tone_end_ms = now + sound_queue[sound_queue_index].duration_ms;
        sound_next_toggle_ms = now;
    }

    if ((int32_t)(now - sound_next_toggle_ms) >= 0) {
        sound_output_high = (uint8_t)!sound_output_high;
        sound_write(sound_output_high ?
                    (SOUND_DAC_CENTER + SOUND_DAC_AMPLITUDE) :
                    (SOUND_DAC_CENTER - SOUND_DAC_AMPLITUDE));
        sound_next_toggle_ms = now + sound_queue[sound_queue_index].half_period_ms;
    }
}
