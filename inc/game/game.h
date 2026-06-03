#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "drivers/joystick.h"
#include "game/game_state.h"

void game_init(void);
void game_start(void);
void game_update(JoystickState joystick, uint8_t fire_pressed);
void game_render(void);
GameState game_get_state(void);
void game_set_state(GameState state);

#endif
