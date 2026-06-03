#include "ui/screen_game_over.h"

#include "drivers/lcd.h"

static uint8_t restart_requested;

void screen_game_over_update(uint8_t select_pressed)
{
    if (select_pressed) {
        restart_requested = 1;
    }
}

void screen_game_over_render(void)
{
    lcd_clear();
    lcd_draw_text(0, 0, "Game Over");
    lcd_draw_text(0, 16, "Press fire");
    lcd_flush();
}

uint8_t screen_game_over_restart_requested(void)
{
    uint8_t requested = restart_requested;

    restart_requested = 0;
    return requested;
}
