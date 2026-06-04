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
    lcd_draw_text(0, 0, "FLASH OK 1600");
    lcd_draw_text(0, 16, "Plane SAFE");
    lcd_draw_text(0, 32, "SEL fire");
    lcd_draw_text(0, 48, "PE3 UP PE6 DN");
    lcd_flush();
}

uint8_t menu_start_requested(void)
{
    uint8_t requested = start_requested;

    start_requested = 0;
    return requested;
}
