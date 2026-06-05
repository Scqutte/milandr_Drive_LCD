#include "game/player.h"

#include "config.h"

void player_init(Player *player)
{
    player->x = PLAYER_START_X;
    player->y = PLAYER_START_Y;
    player->lives = PLAYER_LIVES;
}

void player_update(Player *player, int16_t input_x, int16_t input_y)
{
    player->x += input_x;
    player->y += input_y;

    if (player->x < 0) {
        player->x = 0;
    }

    if (player->y < 0) {
        player->y = 0;
    }

    if (player->x > (SCREEN_WIDTH - PLAYER_WIDTH)) {
        player->x = SCREEN_WIDTH - PLAYER_WIDTH;
    }

    if (player->y > (SCREEN_HEIGHT - PLAYER_HEIGHT)) {
        player->y = SCREEN_HEIGHT - PLAYER_HEIGHT;
    }
}
