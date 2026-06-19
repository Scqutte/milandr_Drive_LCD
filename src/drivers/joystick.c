#include "drivers/joystick.h"

#include "config.h"
#include "drivers/button.h"

#if INPUT_MODE == INPUT_MODE_ADC_JOYSTICK
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define JOYSTICK_ADC_MASK 0x0FFFU

static void joystick_init_adc_pin(uint32_t pin)
{
    PORT_InitTypeDef init;

    init.PORT_Pin = pin;
    init.PORT_OE = PORT_OE_IN;
    init.PORT_PULL_UP = PORT_PULL_UP_OFF;
    init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    init.PORT_PD = PORT_PD_DRIVER;
    init.PORT_GFEN = PORT_GFEN_OFF;
    init.PORT_FUNC = PORT_FUNC_PORT;
    init.PORT_SPEED = PORT_OUTPUT_OFF;
    init.PORT_MODE = PORT_MODE_ANALOG;

    PORT_Init(JOYSTICK_ADC_PORT, &init);
}

static uint16_t joystick_read_adc(uint32_t channel)
{
    uint32_t timeout = JOYSTICK_ADC_TIMEOUT;

    ADC1_SetChannel(channel);
    ADC1_Start();

    while ((ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == RESET) && (timeout > 0U)) {
        timeout--;
    }

    if (timeout == 0U) {
        return JOYSTICK_ADC_CENTER;
    }

    return (uint16_t)(ADC1_GetResult() & JOYSTICK_ADC_MASK);
}

static int16_t joystick_adc_to_step(uint16_t value)
{
    if (value > (JOYSTICK_ADC_CENTER + JOYSTICK_ADC_DEADZONE)) {
        return PLAYER_SPEED;
    }

    if (value < (JOYSTICK_ADC_CENTER - JOYSTICK_ADC_DEADZONE)) {
        return -PLAYER_SPEED;
    }

    return 0;
}
#endif

void joystick_init(void)
{
#if INPUT_MODE == INPUT_MODE_ADC_JOYSTICK
    ADC_InitTypeDef adc_init;
    ADCx_InitTypeDef adc1_init;

    RST_CLK_PCLKcmd(JOYSTICK_ADC_PORT_CLOCK | RST_CLK_PCLK_ADC, ENABLE);

    joystick_init_adc_pin(JOYSTICK_ADC_X_PIN);
    joystick_init_adc_pin(JOYSTICK_ADC_Y_PIN);

    ADC_StructInit(&adc_init);
    ADC_Init(&adc_init);

    ADCx_StructInit(&adc1_init);
    adc1_init.ADC_ChannelNumber = JOYSTICK_ADC_X_CHANNEL;
    adc1_init.ADC_Channels = 0U;
    adc1_init.ADC_SamplingMode = ADC_SAMPLING_MODE_SINGLE_CONV;
    adc1_init.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
    ADC1_Init(&adc1_init);
    ADC1_Cmd(ENABLE);
#endif
}

JoystickState joystick_read(void)
{
    JoystickState state;

    state.x = 0;
    state.y = 0;
    state.pressed = 0;

#if INPUT_MODE == INPUT_MODE_ADC_JOYSTICK
    state.x = joystick_adc_to_step(joystick_read_adc(JOYSTICK_ADC_X_CHANNEL));
    state.y = joystick_adc_to_step(joystick_read_adc(JOYSTICK_ADC_Y_CHANNEL));
#else
    uint8_t buttons = button_state();

    if ((buttons & BUTTON_STATE_UP) != 0U) {
        state.y -= PLAYER_SPEED;
    }

    if ((buttons & BUTTON_STATE_DOWN) != 0U) {
        state.y += PLAYER_SPEED;
    }
#endif

    return state;
}

uint16_t joystick_read_x_raw(void)
{
#if INPUT_MODE == INPUT_MODE_ADC_JOYSTICK
    return joystick_read_adc(JOYSTICK_ADC_X_CHANNEL);
#else
    return JOYSTICK_ADC_CENTER;
#endif
}

uint16_t joystick_read_y_raw(void)
{
#if INPUT_MODE == INPUT_MODE_ADC_JOYSTICK
    return joystick_read_adc(JOYSTICK_ADC_Y_CHANNEL);
#else
    return JOYSTICK_ADC_CENTER;
#endif
}
