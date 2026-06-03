#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include <stdint.h>
#include "config.h"

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t active;
} Enemy;

void enemies_init(Enemy enemies[MAX_ENEMIES]);
void enemies_update(Enemy enemies[MAX_ENEMIES]);

#endif
