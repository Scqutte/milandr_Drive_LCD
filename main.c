#include "app.h"
#include "config.h"
#include "drivers/button.h"
#include "drivers/lcd.h"
#include "drivers/sound.h"
#include "system/board.h"
#include "system/tick.h"

#if APP_MODE == APP_MODE_SOUND_TEST
static void sound_test_show(void)
{
    lcd_clear();
    lcd_draw_text(0U, 0U, "MARIO STYLE");
    lcd_draw_text(0U, 16U, "8BIT LOOP");
    lcd_draw_text(0U, 32U, "short notes");
    lcd_draw_text(0U, 48U, "rests=beat");
    lcd_flush();
}
#endif

int main(void)
{
#if APP_MODE == APP_MODE_SOUND_TEST
    board_init();
    sound_test_show();
    sound_music_start();

    while (1) {
        button_update();
        sound_update();
    }
#else
    app_init();

    while (1) {
        button_update();
        sound_update();

        if (tick_should_run_frame()) {
            app_update();
            app_render();
        }
    }
#endif
}
