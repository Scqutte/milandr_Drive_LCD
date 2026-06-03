#include "ui/hud.h"

#include "drivers/lcd.h"

void hud_render(uint16_t score, uint8_t lives)
{
    (void)score;
    (void)lives;

    lcd_draw_text(0, 0, "Score:");
}
