#include "drivers/button.h"

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define BUTTON_LEFT_PORT MDR_PORTB
#define BUTTON_LEFT_PIN PORT_Pin_4
#define BUTTON_SELECT_PORT MDR_PORTC
#define BUTTON_SELECT_PIN PORT_Pin_0
#define BUTTON_DOWN_PORT MDR_PORTD
#define BUTTON_DOWN_PIN PORT_Pin_0
#define BUTTON_RIGHT_PORT MDR_PORTD
#define BUTTON_RIGHT_PIN PORT_Pin_2
#define BUTTON_UP_PORT MDR_PORTE
#define BUTTON_UP_PIN PORT_Pin_3

static void button_init_pin(MDR_PORT_TypeDef *port, uint32_t pin)
{
    PORT_InitTypeDef init;

    init.PORT_Pin = pin;
    init.PORT_OE = PORT_OE_IN;
    init.PORT_PULL_UP = PORT_PULL_UP_ON;
    init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    init.PORT_PD = PORT_PD_DRIVER;
    init.PORT_GFEN = PORT_GFEN_OFF;
    init.PORT_FUNC = PORT_FUNC_PORT;
    init.PORT_SPEED = PORT_OUTPUT_OFF;
    init.PORT_MODE = PORT_MODE_DIGITAL;

    PORT_Init(port, &init);
}

static uint8_t button_pressed(MDR_PORT_TypeDef *port, uint32_t pin)
{
    return (PORT_ReadInputDataBit(port, pin) == 0U) ? 1U : 0U;
}

void button_init(void)
{
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB |
                    RST_CLK_PCLK_PORTC |
                    RST_CLK_PCLK_PORTD |
                    RST_CLK_PCLK_PORTE,
                    ENABLE);

    button_init_pin(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);
    button_init_pin(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN);
    button_init_pin(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN);
    button_init_pin(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
    button_init_pin(BUTTON_UP_PORT, BUTTON_UP_PIN);
}

uint8_t button_fire_pressed(void)
{
    return button_pressed(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN);
}

uint8_t button_left_pressed(void)
{
    return button_pressed(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);
}

uint8_t button_right_pressed(void)
{
    return button_pressed(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
}

uint8_t button_up_pressed(void)
{
    return button_pressed(BUTTON_UP_PORT, BUTTON_UP_PIN);
}

uint8_t button_down_pressed(void)
{
    return button_pressed(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN);
}
