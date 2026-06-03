#include "game/collision.h"

uint8_t collision_points_overlap(int16_t ax, int16_t ay, int16_t bx, int16_t by)
{
    return (ax == bx) && (ay == by);
}
