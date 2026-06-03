#ifndef UI_SCREEN_WIN_H
#define UI_SCREEN_WIN_H

#include <stdint.h>

void screen_win_update(uint8_t select_pressed);
void screen_win_render(void);
uint8_t screen_win_restart_requested(void);

#endif
