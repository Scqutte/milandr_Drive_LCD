#include "drivers/button.h"

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define FIRE_BUTTON_PORT MDR_PORTD
#define FIRE_BUTTON_PIN PORT_Pin_1

void button_init(void)
{
    PORT_InitTypeDef init;

    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);

    init.PORT_Pin = FIRE_BUTTON_PIN;
    init.PORT_OE = PORT_OE_IN;
    init.PORT_PULL_UP = PORT_PULL_UP_ON;
    init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    init.PORT_PD = PORT_PD_DRIVER;
    init.PORT_GFEN = PORT_GFEN_OFF;
    init.PORT_FUNC = PORT_FUNC_PORT;
    init.PORT_SPEED = PORT_OUTPUT_OFF;
    init.PORT_MODE = PORT_MODE_DIGITAL;

    PORT_Init(FIRE_BUTTON_PORT, &init);
}

uint8_t button_fire_pressed(void)
{
    return (PORT_ReadInputDataBit(FIRE_BUTTON_PORT, FIRE_BUTTON_PIN) == 0U) ? 1U : 0U;
}
