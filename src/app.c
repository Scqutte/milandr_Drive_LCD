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
    static uint8_t previous_fire_pressed;
    JoystickState joystick;
    uint8_t buttons;
    uint8_t fire_pressed;
    uint8_t fire_event;

    buttons = button_state();
    joystick = joystick_read();
    fire_pressed = ((buttons & BUTTON_STATE_FIRE) != 0U) || joystick.pressed;
    fire_event = fire_pressed && !previous_fire_pressed;

    switch (game_get_state()) {
        case GAME_STATE_MENU:
            menu_update(joystick, fire_event);
            if (menu_start_requested()) {
                sound_play_menu();
                game_start();
            }
            break;

        case GAME_STATE_PLAYING:
            game_update(joystick, fire_pressed);
            break;

        case GAME_STATE_WIN:
            screen_win_update(fire_event);
            if (screen_win_restart_requested()) {
                sound_play_menu();
                game_set_state(GAME_STATE_MENU);
            }
            break;

        case GAME_STATE_GAME_OVER:
            screen_game_over_update(fire_event);
            if (screen_game_over_restart_requested()) {
                sound_play_menu();
                game_set_state(GAME_STATE_MENU);
            }
            break;

        default:
            game_set_state(GAME_STATE_MENU);
            break;
    }

    previous_fire_pressed = fire_pressed;
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
