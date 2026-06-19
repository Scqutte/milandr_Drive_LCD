#include "app.h"
#include "config.h"
#include "drivers/button.h"
#include "drivers/joystick.h"
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

#if APP_MODE == APP_MODE_JOYSTICK_TEST
static void joystick_test_draw_u16(uint8_t x, uint8_t y, uint16_t value)
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

static const char *joystick_test_x_dir(int16_t x)
{
    if (x < 0) {
        return "LEFT";
    }

    if (x > 0) {
        return "RIGHT";
    }

    return "CENTER";
}

static const char *joystick_test_y_dir(int16_t y)
{
    if (y < 0) {
        return "UP";
    }

    if (y > 0) {
        return "DOWN";
    }

    return "CENTER";
}

static void joystick_test_render(void)
{
    JoystickState joystick = joystick_read();
    uint16_t x_raw = joystick_read_x_raw();
    uint16_t y_raw = joystick_read_y_raw();

    lcd_clear();
    lcd_draw_text(0U, 0U, "JOY TEST ADC");

    lcd_draw_text(0U, 16U, "X:");
    joystick_test_draw_u16(12U, 16U, x_raw);
    lcd_draw_text(42U, 16U, joystick_test_x_dir(joystick.x));

    lcd_draw_text(0U, 32U, "Y:");
    joystick_test_draw_u16(12U, 32U, y_raw);
    lcd_draw_text(42U, 32U, joystick_test_y_dir(joystick.y));

    lcd_draw_text(0U, 48U, "X24 P5 P6");
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
#elif APP_MODE == APP_MODE_JOYSTICK_TEST
    board_init();

    while (1) {
        button_update();
        sound_update();

        if (tick_should_run_frame()) {
            joystick_test_render();
        }
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
