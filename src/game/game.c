#include "game/game.h"

#include "config.h"
#include "drivers/lcd.h"
#include "drivers/sound.h"
#include "game/bullet.h"
#include "game/collision.h"
#include "game/enemy.h"
#include "game/player.h"
#include "game/score.h"
#include "system/tick.h"
#include "ui/hud.h"

static GameState state;
static Player player;
static Bullet bullets[MAX_BULLETS];
static Enemy enemies[MAX_ENEMIES];
static uint32_t next_shot_ms;

static int16_t next_enemy_y(void)
{
    static uint8_t index;
    static const int16_t positions[MAX_ENEMIES] = {12, 20, 28, 36, 44, 52, 16, 40};

    int16_t y = positions[index % MAX_ENEMIES];

    index++;
    return y;
}

static void respawn_enemy(Enemy *enemy)
{
    enemy->active = 1;
    enemy->x = SCREEN_WIDTH - ENEMY_WIDTH;
    enemy->y = next_enemy_y();
}

static void draw_player(void)
{
    lcd_draw_pixel((uint8_t)(player.x + 0), (uint8_t)(player.y + 1), 1);
    lcd_draw_pixel((uint8_t)(player.x + 1), (uint8_t)(player.y + 0), 1);
    lcd_draw_pixel((uint8_t)(player.x + 1), (uint8_t)(player.y + 1), 1);
    lcd_draw_pixel((uint8_t)(player.x + 1), (uint8_t)(player.y + 2), 1);
    lcd_draw_pixel((uint8_t)(player.x + 2), (uint8_t)(player.y + 1), 1);
    lcd_draw_pixel((uint8_t)(player.x + 3), (uint8_t)(player.y + 1), 1);
    lcd_draw_pixel((uint8_t)(player.x + 4), (uint8_t)(player.y + 1), 1);
}

static void draw_enemy(const Enemy *enemy)
{
    lcd_draw_pixel((uint8_t)(enemy->x + 0), (uint8_t)(enemy->y + 0), 1);
    lcd_draw_pixel((uint8_t)(enemy->x + 1), (uint8_t)(enemy->y + 1), 1);
    lcd_draw_pixel((uint8_t)(enemy->x + 2), (uint8_t)(enemy->y + 0), 1);
    lcd_draw_pixel((uint8_t)(enemy->x + 0), (uint8_t)(enemy->y + 2), 1);
    lcd_draw_pixel((uint8_t)(enemy->x + 2), (uint8_t)(enemy->y + 2), 1);
}

static void update_collisions(void)
{
    uint8_t enemy_index;
    uint8_t bullet_index;

    for (enemy_index = 0; enemy_index < MAX_ENEMIES; enemy_index++) {
        if (!enemies[enemy_index].active) {
            continue;
        }

        for (bullet_index = 0; bullet_index < MAX_BULLETS; bullet_index++) {
            if (bullets[bullet_index].active &&
                collision_rects_overlap(bullets[bullet_index].x,
                                        bullets[bullet_index].y,
                                        1,
                                        1,
                                        enemies[enemy_index].x,
                                        enemies[enemy_index].y,
                                        ENEMY_WIDTH,
                                        ENEMY_HEIGHT)) {
                bullets[bullet_index].active = 0;
                score_add(ENEMY_SCORE_VALUE);
                respawn_enemy(&enemies[enemy_index]);
                sound_play_explosion();
            }
        }

        if (collision_rects_overlap(player.x,
                                    player.y,
                                    PLAYER_WIDTH,
                                    PLAYER_HEIGHT,
                                    enemies[enemy_index].x,
                                    enemies[enemy_index].y,
                                    ENEMY_WIDTH,
                                    ENEMY_HEIGHT) ||
            (enemies[enemy_index].x <= 0)) {
            if (player.lives > 0U) {
                player.lives--;
            }

            respawn_enemy(&enemies[enemy_index]);

            if (player.lives == 0U) {
                state = GAME_STATE_GAME_OVER;
                sound_play_explosion();
                return;
            }
        }
    }

    if (score_get() >= WIN_SCORE) {
        state = GAME_STATE_WIN;
        sound_play_win();
    }
}

void game_init(void)
{
    state = GAME_STATE_MENU;
    player_init(&player);
    bullets_init(bullets);
    enemies_init(enemies);
    score_reset();
    next_shot_ms = 0U;
}

void game_start(void)
{
    state = GAME_STATE_PLAYING;
    player_init(&player);
    bullets_init(bullets);
    enemies_init(enemies);
    score_reset();
    next_shot_ms = 0U;
}

void game_update(JoystickState joystick, uint8_t fire_pressed)
{
    uint32_t now;

    if (state != GAME_STATE_PLAYING) {
        return;
    }

    player_update(&player, joystick.x, joystick.y);
    now = tick_get_ms();

    if (fire_pressed && ((int32_t)(now - next_shot_ms) >= 0)) {
        next_shot_ms = now + PLAYER_SHOT_COOLDOWN_MS;

        if (bullets_fire(bullets, player.x + 1, player.y)) {
            sound_play_shot();
        }
    }

    bullets_update(bullets);
    enemies_update(enemies);
    update_collisions();
}

void game_render(void)
{
    uint8_t i;

    lcd_clear();
    hud_render(score_get(), player.lives);
    draw_player();

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            lcd_draw_pixel((uint8_t)bullets[i].x, (uint8_t)bullets[i].y, 1);
        }
    }

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            draw_enemy(&enemies[i]);
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
