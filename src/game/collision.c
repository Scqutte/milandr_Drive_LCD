#include "game/collision.h"

uint8_t collision_points_overlap(int16_t ax, int16_t ay, int16_t bx, int16_t by)
{
    return (ax == bx) && (ay == by);
}

uint8_t collision_rects_overlap(int16_t ax,
                                int16_t ay,
                                int16_t aw,
                                int16_t ah,
                                int16_t bx,
                                int16_t by,
                                int16_t bw,
                                int16_t bh)
{
    return (ax < (bx + bw)) && ((ax + aw) > bx) && (ay < (by + bh)) && ((ay + ah) > by);
}
