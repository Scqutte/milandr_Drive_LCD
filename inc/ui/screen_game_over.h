#ifndef UI_SCREEN_GAME_OVER_H
#define UI_SCREEN_GAME_OVER_H

#include <stdint.h>

void screen_game_over_update(uint8_t select_pressed);
void screen_game_over_render(void);
uint8_t screen_game_over_restart_requested(void);

#endif
