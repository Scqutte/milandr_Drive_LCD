#include "drivers/button.h"

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "system/tick.h"

#define BUTTON_SELECT_PORT MDR_PORTC
#define BUTTON_SELECT_PIN PORT_Pin_0
#define BUTTON_DOWN_PORT MDR_PORTE
#define BUTTON_DOWN_PIN PORT_Pin_6
#define BUTTON_UP_PORT MDR_PORTE
#define BUTTON_UP_PIN PORT_Pin_3
#define BUTTON_DEBOUNCE_MS 20U

typedef struct {
    MDR_PORT_TypeDef *port;
    uint32_t pin;
    uint8_t raw_pressed;
    uint8_t stable_pressed;
    uint32_t changed_at_ms;
} ButtonDebounce;

static ButtonDebounce buttons[] = {
    {BUTTON_SELECT_PORT, BUTTON_SELECT_PIN, 0U, 0U, 0U},
    {BUTTON_UP_PORT, BUTTON_UP_PIN, 0U, 0U, 0U},
    {BUTTON_DOWN_PORT, BUTTON_DOWN_PIN, 0U, 0U, 0U},
};

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

static uint8_t button_read_raw(MDR_PORT_TypeDef *port, uint32_t pin)
{
    return (PORT_ReadInputDataBit(port, pin) == 0U) ? 1U : 0U;
}

static uint8_t button_pressed(uint8_t index)
{
    return buttons[index].stable_pressed;
}

void button_init(void)
{
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC |
                    RST_CLK_PCLK_PORTE,
                    ENABLE);

    button_init_pin(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN);
    button_init_pin(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN);
    button_init_pin(BUTTON_UP_PORT, BUTTON_UP_PIN);
    button_update();
}

void button_update(void)
{
    uint8_t i;
    uint32_t now = tick_get_ms();

    for (i = 0U; i < (uint8_t)(sizeof(buttons) / sizeof(buttons[0])); i++) {
        uint8_t raw_pressed = button_read_raw(buttons[i].port, buttons[i].pin);

        if (raw_pressed != buttons[i].raw_pressed) {
            buttons[i].raw_pressed = raw_pressed;
            buttons[i].changed_at_ms = now;
        }

        if ((now - buttons[i].changed_at_ms) >= BUTTON_DEBOUNCE_MS) {
            buttons[i].stable_pressed = buttons[i].raw_pressed;
        }
    }
}

uint8_t button_fire_pressed(void)
{
    return button_pressed(0U);
}

uint8_t button_left_pressed(void)
{
    return 0U;
}

uint8_t button_right_pressed(void)
{
    return 0U;
}

uint8_t button_up_pressed(void)
{
    return button_pressed(1U);
}

uint8_t button_down_pressed(void)
{
    return button_pressed(2U);
}
