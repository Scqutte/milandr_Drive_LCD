#include "ui/menu.h"

#include "drivers/joystick.h"
#include "drivers/lcd.h"

static uint8_t start_requested;

static void menu_draw_u16(uint8_t x, uint8_t y, uint16_t value)
{
    char text[5];

    if (value > 9999U) {
        value = 9999U;
    }

    text[0] = (char)('0' + ((value / 1000U) % 10U));
    text[1] = (char)('0' + ((value / 100U) % 10U));
    text[2] = (char)('0' + ((value / 10U) % 10U));
    text[3] = (char)('0' + (value % 10U));
    text[4] = '\0';

    lcd_draw_text(x, y, text);
}

void menu_init(void)
{
    start_requested = 0;
}

void menu_update(JoystickState joystick, uint8_t select_pressed)
{
    (void)joystick;

    if (select_pressed) {
        start_requested = 1;
    }
}

void menu_render(void)
{
    uint16_t x_raw = joystick_read_x_raw();
    uint16_t y_raw = joystick_read_y_raw();

    lcd_clear();
    lcd_draw_text(0, 0, "PLANE GAME");
    lcd_draw_text(0, 16, "X:");
    menu_draw_u16(12, 16, x_raw);
    lcd_draw_text(0, 32, "Y:");
    menu_draw_u16(12, 32, y_raw);
    lcd_draw_text(0, 48, "SEL START");
    lcd_flush();
}

uint8_t menu_start_requested(void)
{
    uint8_t requested = start_requested;

    start_requested = 0;
    return requested;
}
