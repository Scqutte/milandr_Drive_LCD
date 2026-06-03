#include "game/game.h"

#include "config.h"
#include "drivers/lcd.h"
#include "drivers/sound.h"
#include "game/bullet.h"
#include "game/enemy.h"
#include "game/player.h"
#include "game/score.h"
#include "ui/hud.h"

static GameState state;
static Player player;
static Bullet bullets[MAX_BULLETS];
static Enemy enemies[MAX_ENEMIES];

void game_init(void)
{
    state = GAME_STATE_MENU;
    player_init(&player);
    bullets_init(bullets);
    enemies_init(enemies);
    score_reset();
}

void game_start(void)
{
    state = GAME_STATE_PLAYING;
    player_init(&player);
    bullets_init(bullets);
    enemies_init(enemies);
    score_reset();
}

void game_update(JoystickState joystick, uint8_t fire_pressed)
{
    if (state != GAME_STATE_PLAYING) {
        return;
    }

    player_update(&player, joystick.x, joystick.y);

    if (fire_pressed) {
        bullets_fire(bullets, player.x + 1, player.y);
        sound_play_shot();
    }

    bullets_update(bullets);
    enemies_update(enemies);
}

void game_render(void)
{
    uint8_t i;

    lcd_clear();
    hud_render(score_get(), player.lives);
    lcd_draw_pixel((uint8_t)player.x, (uint8_t)player.y, 1);

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            lcd_draw_pixel((uint8_t)bullets[i].x, (uint8_t)bullets[i].y, 1);
        }
    }

    lcd_flush();
}

GameState game_get_state(void)
{
    return state;
}

void game_set_state(GameState new_state)
{
    state = new_state;
}
