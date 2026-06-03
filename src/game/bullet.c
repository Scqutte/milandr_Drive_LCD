#include "game/bullet.h"

#include "config.h"

void bullets_init(Bullet bullets[MAX_BULLETS])
{
    uint8_t i;

    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
        bullets[i].x = 0;
        bullets[i].y = 0;
    }
}

void bullets_fire(Bullet bullets[MAX_BULLETS], int16_t x, int16_t y)
{
    uint8_t i;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = 1;
            bullets[i].x = x;
            bullets[i].y = y;
            return;
        }
    }
}

void bullets_update(Bullet bullets[MAX_BULLETS])
{
    uint8_t i;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].x += BULLET_SPEED;

            if (bullets[i].x >= SCREEN_WIDTH) {
                bullets[i].active = 0;
            }
        }
    }
}
