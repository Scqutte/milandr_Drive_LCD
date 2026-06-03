#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t lives;
} Player;

void player_init(Player *player);
void player_update(Player *player, int16_t input_x, int16_t input_y);

#endif
