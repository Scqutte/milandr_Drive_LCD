#include "app.h"

#include "drivers/button.h"
#include "drivers/joystick.h"
#include "drivers/sound.h"
#include "game/game.h"
#include "system/board.h"
#include "ui/menu.h"
#include "ui/screen_game_over.h"
#include "ui/screen_win.h"

void app_init(void)
{
    board_init();
    menu_init();
    game_init();
}

void app_update(void)
{
    JoystickState joystick = joystick_read();
    uint8_t fire_pressed = button_fire_pressed() || joystick.pressed;

    switch (game_get_state()) {
        case GAME_STATE_MENU:
            menu_update(joystick, fire_pressed);
            if (menu_start_requested()) {
                game_start();
            }
            break;

        case GAME_STATE_PLAYING:
            game_update(joystick, fire_pressed);
            break;

        case GAME_STATE_WIN:
            screen_win_update(fire_pressed);
            if (screen_win_restart_requested()) {
                game_set_state(GAME_STATE_MENU);
            }
            break;

        case GAME_STATE_GAME_OVER:
            screen_game_over_update(fire_pressed);
            if (screen_game_over_restart_requested()) {
                game_set_state(GAME_STATE_MENU);
            }
            break;

        default:
            game_set_state(GAME_STATE_MENU);
            break;
    }

    sound_update();
}

void app_render(void)
{
    switch (game_get_state()) {
        case GAME_STATE_MENU:
            menu_render();
            break;

        case GAME_STATE_PLAYING:
            game_render();
            break;

        case GAME_STATE_WIN:
            screen_win_render();
            break;

        case GAME_STATE_GAME_OVER:
            screen_game_over_render();
            break;

        default:
            break;
    }
}
