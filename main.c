#include "app.h"
#include "drivers/button.h"
#include "drivers/sound.h"
#include "system/tick.h"

int main(void)
{
    app_init();

    while (1) {
        button_update();
        sound_update();

        if (tick_should_run_frame()) {
            app_update();
            app_render();
        }
    }
}
