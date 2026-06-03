#include "game/score.h"

static uint16_t current_score;

void score_reset(void)
{
    current_score = 0;
}

void score_add(uint16_t value)
{
    current_score += value;
}

uint16_t score_get(void)
{
    return current_score;
}
