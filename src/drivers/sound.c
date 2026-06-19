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
#define SOUND_QUEUE_MAX 8U
#define NOTE_REST 0U
#define NOTE_C5 956U
#define NOTE_D5 852U
#define NOTE_E5 758U
#define NOTE_F5 716U
#define NOTE_G5 638U
#define NOTE_A5 568U
#define NOTE_B5 506U
#define NOTE_C6 478U
#define NOTE_D6 426U
#define NOTE_E6 379U
#define NOTE_G6 319U
#define NOTE_E4 1517U
#define NOTE_G4 1276U
#define NOTE_A4 1136U
#define NOTE_AS4 1073U   
#define NOTE_B4 1012U

typedef struct {
    uint16_t half_period_us;
    uint16_t duration_ms;
} SoundTone;

static SoundTone sound_queue[SOUND_QUEUE_MAX];
static uint8_t sound_queue_count;
static uint8_t sound_queue_index;
static uint8_t sound_output_high;
static uint8_t sound_active;
static uint8_t sound_sequence_is_music;
static uint8_t sound_music_enabled;
static uint16_t sound_music_step;
static uint32_t sound_next_toggle_us;
static uint32_t sound_tone_end_us;
static uint32_t sound_next_music_ms;

static const SoundTone sound_music_loop[] = {
    {NOTE_E5, 85U},  {NOTE_REST, 45U},
    {NOTE_E5, 85U},  {NOTE_REST, 120U},
    {NOTE_E5, 85U},  {NOTE_REST, 120U},

    {NOTE_C5, 85U},  {NOTE_REST, 45U},
    {NOTE_E5, 85U},  {NOTE_REST, 120U},

    {NOTE_G5, 170U}, {NOTE_REST, 280U},
    {NOTE_G4, 170U}, {NOTE_REST, 280U},

    // C G E
    {NOTE_C5, 170U}, {NOTE_REST, 170U},
    {NOTE_G4, 170U}, {NOTE_REST, 170U},
    {NOTE_E4, 170U}, {NOTE_REST, 170U},

    // A B Bb A
    {NOTE_A4, 170U},  {NOTE_REST, 45U},
    {NOTE_B4, 170U},  {NOTE_REST, 45U},
    {NOTE_AS4, 85U},  {NOTE_REST, 30U},
    {NOTE_A4, 170U},  {NOTE_REST, 120U},

    // G E G A F G
    {NOTE_G4, 226U},
    {NOTE_E5, 226U},
    {NOTE_G5, 226U},
    {NOTE_A5, 340U},  

    {NOTE_F5, 170U},   
    {NOTE_G5, 170U},   {NOTE_REST, 85U},

    // E C D B
    {NOTE_E5, 170U},  
    {NOTE_C5, 85U},   
    {NOTE_D5, 85U},   
    {NOTE_B4, 170U},  {NOTE_REST, 85U},
    // x2
    // C G E
    {NOTE_C5, 170U}, {NOTE_REST, 170U},
    {NOTE_G4, 170U}, {NOTE_REST, 170U},
    {NOTE_E4, 170U}, {NOTE_REST, 170U},

    // A B Bb A
    {NOTE_A4, 170U},  {NOTE_REST, 45U},
    {NOTE_B4, 170U},  {NOTE_REST, 45U},
    {NOTE_AS4, 85U},  {NOTE_REST, 30U},
    {NOTE_A4, 170U},  {NOTE_REST, 120U},

    // G E G A F G
    {NOTE_G4, 226U},
    {NOTE_E5, 226U},
    {NOTE_G5, 226U},
    {NOTE_A5, 340U},  

    {NOTE_F5, 170U},   
    {NOTE_G5, 170U},   {NOTE_REST, 85U},

    // E C D B
    {NOTE_E5, 170U},  
    {NOTE_C5, 85U},   
    {NOTE_D5, 85U},   
    {NOTE_B4, 170U},  {NOTE_REST, 85U},

};

#define SOUND_MUSIC_STEPS ((uint16_t)(sizeof(sound_music_loop) / sizeof(sound_music_loop[0])))

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
    sound_sequence_is_music = 0U;
    sound_music_enabled = 0U;
    sound_music_step = 0U;
    sound_next_music_ms = 0U;
    sound_write(SOUND_DAC_CENTER);
}

static void sound_start_sequence(const SoundTone *tones, uint8_t count, uint8_t is_music)
{
    uint16_t i;
    uint32_t now_us = tick_get_us();

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
    sound_sequence_is_music = is_music;
    sound_next_toggle_us = now_us;
    sound_tone_end_us = now_us + ((uint32_t)sound_queue[0].duration_ms * 1000U);

    if (sound_queue[0].half_period_us == NOTE_REST) {
        sound_write(SOUND_DAC_CENTER);
    }
}

void sound_music_start(void)
{
    sound_music_enabled = 1U;
    sound_music_step = 0U;
    sound_next_music_ms = tick_get_ms();
}

void sound_music_stop(void)
{
    sound_music_enabled = 0U;

    if (sound_sequence_is_music) {
        sound_active = 0U;
        sound_write(SOUND_DAC_CENTER);
    }
}

void sound_play_menu(void)
{
    static const SoundTone tones[] = {
        {650U, 35U},
        {430U, 45U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_shot(void)
{
    static const SoundTone tones[] = {
        {500U, 55U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_enemy_hit(void)
{
    static const SoundTone tones[] = {
        {1500U, 45U},
        {1050U, 55U},
        {700U, 65U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_life_lost(void)
{
    static const SoundTone tones[] = {
        {420U, 70U},
        {760U, 90U},
        {1200U, 120U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_game_over(void)
{
    static const SoundTone tones[] = {
        {480U, 100U},
        {680U, 110U},
        {960U, 130U},
        {1400U, 180U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_win(void)
{
    static const SoundTone tones[] = {
        {700U, 80U},
        {560U, 80U},
        {470U, 80U},
        {350U, 160U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_test_low(void)
{
    static const SoundTone tones[] = {
        {1000U, 900U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_test_high(void)
{
    static const SoundTone tones[] = {
        {500U, 900U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

void sound_play_test_scale(void)
{
    static const SoundTone tones[] = {
        {956U, 180U},
        {852U, 180U},
        {758U, 180U},
        {716U, 180U},
        {638U, 180U},
        {568U, 180U},
        {506U, 180U},
        {478U, 260U},
    };

    sound_start_sequence(tones, (uint8_t)(sizeof(tones) / sizeof(tones[0])), 0U);
}

static void sound_update_music(void)
{
    uint32_t now_ms;

    if (!sound_music_enabled || sound_active) {
        return;
    }

    now_ms = tick_get_ms();

    if ((int32_t)(now_ms - sound_next_music_ms) < 0) {
        return;
    }

    sound_start_sequence(&sound_music_loop[sound_music_step], 1U, 1U);
    sound_next_music_ms = now_ms + sound_music_loop[sound_music_step].duration_ms;
    sound_music_step++;

    if (sound_music_step >= SOUND_MUSIC_STEPS) {
        sound_music_step = 0U;
    }
}

void sound_update(void)
{
    uint32_t now_us;

    if (!sound_active) {
        sound_update_music();
        return;
    }

    now_us = tick_get_us();

    if ((int32_t)(now_us - sound_tone_end_us) >= 0) {
        sound_queue_index++;

        if (sound_queue_index >= sound_queue_count) {
            sound_active = 0U;
            sound_sequence_is_music = 0U;
            sound_write(SOUND_DAC_CENTER);
            sound_update_music();
            return;
        }

        sound_tone_end_us = now_us + ((uint32_t)sound_queue[sound_queue_index].duration_ms * 1000U);
        sound_next_toggle_us = now_us;

        if (sound_queue[sound_queue_index].half_period_us == NOTE_REST) {
            sound_write(SOUND_DAC_CENTER);
        }
    }

    if (sound_queue[sound_queue_index].half_period_us == NOTE_REST) {
        return;
    }

    if ((int32_t)(now_us - sound_next_toggle_us) >= 0) {
        sound_output_high = (uint8_t)!sound_output_high;
        sound_write(sound_output_high ?
                    (SOUND_DAC_CENTER + SOUND_DAC_AMPLITUDE) :
                    (SOUND_DAC_CENTER - SOUND_DAC_AMPLITUDE));
        sound_next_toggle_us = now_us + sound_queue[sound_queue_index].half_period_us;
    }
}
