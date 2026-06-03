#ifndef GAME_BULLET_H
#define GAME_BULLET_H

#include <stdint.h>
#include "config.h"

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t active;
} Bullet;

void bullets_init(Bullet bullets[MAX_BULLETS]);
void bullets_fire(Bullet bullets[MAX_BULLETS], int16_t x, int16_t y);
void bullets_update(Bullet bullets[MAX_BULLETS]);

#endif
