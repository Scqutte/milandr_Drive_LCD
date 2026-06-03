#include "game/enemy.h"

#include "config.h"

void enemies_init(Enemy enemies[MAX_ENEMIES])
{
    uint8_t i;

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].x = 0;
        enemies[i].y = 0;
    }
}

void enemies_update(Enemy enemies[MAX_ENEMIES])
{
    uint8_t i;

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active && enemies[i].x > 0) {
            enemies[i].x--;
        }
    }
}
