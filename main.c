#include "app.h"
#include "drivers/lcd.h"
#include "drivers/sound.h"
#include "system/board.h"
#include "system/tick.h"

#define RUN_SOUND_PARTY_TEST 0

int main(void)
{
#if RUN_SOUND_PARTY_TEST
    board_init();
    sound_set_test_route(1U);

    while (1) {
        lcd_clear();
        lcd_draw_text(0, 0, "AUDIO V7");
        lcd_draw_text(0, 16, "DAC2 ONLY");
        lcd_draw_text(0, 32, "LOUD TONE");
        lcd_draw_text(0, 48, "MAX DAC");
        lcd_flush();

        sound_play_test_party();
        tick_delay_ms(120);
    }
#else
    app_init();

    while (1) {
        if (tick_should_run_frame()) {
            app_update();
            app_render();
        }
    }
#endif
}
