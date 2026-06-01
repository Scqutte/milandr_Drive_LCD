#include <stdint.h>

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"

#define LED_PORT MDR_PORTA

#define LED_PINS (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | \
                  PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7)

static void delay(volatile uint32_t value)
{
    while (value--) {
    }
}

static void init_leds(void)
{
    PORT_InitTypeDef port_init;

    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);

    port_init.PORT_Pin = LED_PINS;
    port_init.PORT_OE = PORT_OE_OUT;
    port_init.PORT_PULL_UP = PORT_PULL_UP_OFF;
    port_init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    port_init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    port_init.PORT_PD = PORT_PD_DRIVER;
    port_init.PORT_GFEN = PORT_GFEN_OFF;
    port_init.PORT_FUNC = PORT_FUNC_PORT;
    port_init.PORT_SPEED = PORT_SPEED_SLOW;
    port_init.PORT_MODE = PORT_MODE_DIGITAL;

    PORT_Init(LED_PORT, &port_init);
}

int main(void)
{
    init_leds();

    while (1) {
        PORT_SetBits(LED_PORT, LED_PINS);
        delay(500000);

        PORT_ResetBits(LED_PORT, LED_PINS);
        delay(500000);
    }
}