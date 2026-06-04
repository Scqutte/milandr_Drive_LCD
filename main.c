#include "app.h"
#include "drivers/lcd.h"
#include "system/tick.h"

int main(void)
{
    uint8_t x;
    uint8_t y;

    app_init();

    lcd_fill_raw(0x00U);
    lcd_clear();

    for (x = 0U; x < 128U; x++) {
        lcd_draw_pixel(x, 0U, 1U);
        lcd_draw_pixel(x, 63U, 1U);
    }

    for (y = 0U; y < 64U; y++) {
        lcd_draw_pixel(0U, y, 1U);
        lcd_draw_pixel(127U, y, 1U);
    }

    lcd_draw_text(18U, 12U, "LCD OK");
    lcd_draw_text(18U, 28U, "MILANDR");
    lcd_draw_text(18U, 44U, "READY");
    lcd_flush();

    while (1) {
        tick_delay_ms(1000U);
    }
}
