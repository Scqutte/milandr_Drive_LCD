#ifndef DRIVERS_SOUND_H
#define DRIVERS_SOUND_H

#include <stdint.h>

void sound_init(void);
void sound_music_start(void);
void sound_music_stop(void);
void sound_play_menu(void);
void sound_play_shot(void);
void sound_play_enemy_hit(void);
void sound_play_life_lost(void);
void sound_play_game_over(void);
void sound_play_win(void);
void sound_play_test_low(void);
void sound_play_test_high(void);
void sound_play_test_scale(void);
void sound_update(void);
void sound_tick_100us(void);

#endif
