#include "ui/menu.h"

#include "drivers/lcd.h"

static uint8_t start_requested;

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
    lcd_clear();
    lcd_draw_text(0, 0, "Milandr Plane");
    lcd_draw_text(0, 16, "Start");
    lcd_flush();
}

uint8_t menu_start_requested(void)
{
    uint8_t requested = start_requested;

    start_requested = 0;
    return requested;
}
