#include "app.h"
#include "drivers/sound.h"
#include "system/tick.h"

int main(void)
{
    app_init();

    while (1) {
        sound_update();

        if (tick_should_run_frame()) {
            app_update();
            app_render();
        }
    }
}
