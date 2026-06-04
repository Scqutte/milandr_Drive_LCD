#include "drivers/joystick.h"

#include "config.h"
#include "drivers/button.h"

void joystick_init(void)
{
}

JoystickState joystick_read(void)
{
    JoystickState state;

    state.x = 0;
    state.y = 0;
    state.pressed = 0;

    if (button_left_pressed()) {
        state.x -= PLAYER_SPEED;
    }

    if (button_right_pressed()) {
        state.x += PLAYER_SPEED;
    }

    if (button_up_pressed()) {
        state.y -= PLAYER_SPEED;
    }

    if (button_down_pressed()) {
        state.y += PLAYER_SPEED;
    }

    return state;
}
