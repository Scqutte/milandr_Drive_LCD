#include "game/enemy.h"

#include "config.h"

static uint8_t enemy_spawn_index;

static int16_t enemy_spawn_y(uint8_t index)
{
    static const int16_t positions[MAX_ENEMIES] = {12, 20, 28, 36, 44, 52, 16, 40};

    return positions[index % MAX_ENEMIES];
}

static void enemy_spawn(Enemy *enemy, uint8_t index)
{
    enemy->active = 1;
    enemy->x = (int16_t)(SCREEN_WIDTH - 1 + ((index % 4U) * 18U));
    enemy->y = enemy_spawn_y((uint8_t)(enemy_spawn_index + index));
}

void enemies_init(Enemy enemies[MAX_ENEMIES])
{
    uint8_t i;

    enemy_spawn_index = 0;

    for (i = 0; i < MAX_ENEMIES; i++) {
        enemy_spawn(&enemies[i], i);
    }
}

void enemies_update(Enemy enemies[MAX_ENEMIES])
{
    uint8_t i;

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].x -= ENEMY_SPEED;
        }
    }
}
