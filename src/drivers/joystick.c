#include "drivers/joystick.h"

#include "config.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define JOYSTICK_PORT MDR_PORTD
#define JOYSTICK_AXIS_PINS (PORT_Pin_0 | PORT_Pin_3)
#define ADC_RESULT_MASK 0x0FFFU
#define ADC_TIMEOUT 100000U

static void joystick_init_axis_pins(void)
{
    PORT_InitTypeDef init;

    init.PORT_Pin = JOYSTICK_AXIS_PINS;
    init.PORT_OE = PORT_OE_IN;
    init.PORT_PULL_UP = PORT_PULL_UP_OFF;
    init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    init.PORT_PD = PORT_PD_DRIVER;
    init.PORT_GFEN = PORT_GFEN_OFF;
    init.PORT_FUNC = PORT_FUNC_PORT;
    init.PORT_SPEED = PORT_OUTPUT_OFF;
    init.PORT_MODE = PORT_MODE_ANALOG;

    PORT_Init(JOYSTICK_PORT, &init);
}

static uint16_t joystick_read_adc(uint32_t channel)
{
    uint32_t timeout = ADC_TIMEOUT;

    ADC1_SetChannel(channel);
    ADC1_Start();

    while ((ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET) && (timeout > 0U)) {
        timeout--;
    }

    return (uint16_t)(ADC1_GetResult() & ADC_RESULT_MASK);
}

static int16_t joystick_axis_to_step(uint16_t value)
{
    if (value > (JOYSTICK_CENTER + JOYSTICK_DEADZONE)) {
        return PLAYER_SPEED;
    }

    if (value < (JOYSTICK_CENTER - JOYSTICK_DEADZONE)) {
        return -PLAYER_SPEED;
    }

    return 0;
}

void joystick_init(void)
{
    ADC_InitTypeDef adc_init;
    ADCx_InitTypeDef adc1_init;

    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD | RST_CLK_PCLK_ADC, ENABLE);
    RST_CLK_ADCclkSelection(RST_CLK_ADCclkCPU_C1);
    RST_CLK_ADCclkPrescaler(RST_CLK_ADCclkDIV16);
    RST_CLK_ADCclkEnable(ENABLE);

    joystick_init_axis_pins();

    ADC_StructInit(&adc_init);
    ADC_Init(&adc_init);

    ADCx_StructInit(&adc1_init);
    adc1_init.ADC_ClockSource = ADC_CLOCK_SOURCE_ADC;
    adc1_init.ADC_SamplingMode = ADC_SAMPLING_MODE_SINGLE_CONV;
    adc1_init.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
    adc1_init.ADC_ChannelNumber = JOYSTICK_X_ADC_CHANNEL;
    adc1_init.ADC_Channels = 0;
    adc1_init.ADC_LevelControl = ADC_LEVEL_CONTROL_Disable;
    adc1_init.ADC_LowLevel = 0;
    adc1_init.ADC_HighLevel = 0x07FF;
    adc1_init.ADC_VRefSource = ADC_VREF_SOURCE_INTERNAL;
    adc1_init.ADC_IntVRefSource = ADC_INT_VREF_SOURCE_INEXACT;
    adc1_init.ADC_Prescaler = ADC_CLK_div_16;
    adc1_init.ADC_DelayGo = 0;

    ADC1_Init(&adc1_init);
    ADC1_Cmd(ENABLE);
}

JoystickState joystick_read(void)
{
    JoystickState state;
    uint16_t raw_x = joystick_read_adc(JOYSTICK_X_ADC_CHANNEL);
    uint16_t raw_y = joystick_read_adc(JOYSTICK_Y_ADC_CHANNEL);

    state.x = joystick_axis_to_step(raw_x);
    state.y = joystick_axis_to_step(raw_y);
    state.pressed = 0;

    return state;
}
