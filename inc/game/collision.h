#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <stdint.h>

uint8_t collision_points_overlap(int16_t ax, int16_t ay, int16_t bx, int16_t by);
uint8_t collision_rects_overlap(int16_t ax,
                                int16_t ay,
                                int16_t aw,
                                int16_t ah,
                                int16_t bx,
                                int16_t by,
                                int16_t bw,
                                int16_t bh);

#endif
