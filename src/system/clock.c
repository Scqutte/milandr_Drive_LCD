#include "system/clock.h"

#include <stdint.h>

#include "system_MDR32F9Qx.h"

void clock_init(void)
{
    SystemCoreClockUpdate();
}
