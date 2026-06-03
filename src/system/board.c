#include "system/board.h"

#include "drivers/button.h"
#include "drivers/joystick.h"
#include "drivers/lcd.h"
#include "drivers/sound.h"
#include "system/clock.h"
#include "system/tick.h"

void board_init(void)
{
    clock_init();
    tick_init();
    lcd_init();
    joystick_init();
    button_init();
    sound_init();
}
