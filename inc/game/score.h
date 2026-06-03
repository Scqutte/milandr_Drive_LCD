#ifndef GAME_SCORE_H
#define GAME_SCORE_H

#include <stdint.h>

void score_reset(void);
void score_add(uint16_t value);
uint16_t score_get(void);

#endif
