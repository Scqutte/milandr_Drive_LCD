#include "ui/hud.h"

#include "drivers/lcd.h"

static void hud_u16_to_text(uint16_t value, char *text)
{
    text[0] = (char)('0' + ((value / 1000U) % 10U));
    text[1] = (char)('0' + ((value / 100U) % 10U));
    text[2] = (char)('0' + ((value / 10U) % 10U));
    text[3] = (char)('0' + (value % 10U));
    text[4] = '\0';
}

void hud_render(uint16_t score, uint8_t lives)
{
    char score_text[5];
    char lives_text[2];

    hud_u16_to_text(score, score_text);
    lives_text[0] = (char)('0' + (lives % 10U));
    lives_text[1] = '\0';

    lcd_draw_text(0, 0, "Score:");
    lcd_draw_text(42, 0, score_text);
    lcd_draw_text(96, 0, "L:");
    lcd_draw_text(108, 0, lives_text);
}
