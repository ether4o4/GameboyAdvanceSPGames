#ifndef TRASH_LID_DASH_GAME_MODEL_H
#define TRASH_LID_DASH_GAME_MODEL_H

#include <stdint.h>

enum GameButton {
    GAME_LEFT   = 1u << 0,
    GAME_RIGHT  = 1u << 1,
    GAME_JUMP   = 1u << 2,
    GAME_START  = 1u << 3,
    GAME_SELECT = 1u << 4
};

enum GameState {
    GAME_TITLE,
    GAME_PLAYING,
    GAME_WIPEOUT,
    GAME_FINISHED
};

typedef struct Game {
    int32_t x;       /* world position, 8.8 fixed point */
    int32_t y;
    int32_t vx;
    int32_t vy;
    int camera;
    int distance;
    int grounded;
    int coyote_frames;
    int jump_buffer_frames;
    int state;
} Game;

void game_reset(Game *game);
void game_update(Game *game, uint16_t held, uint16_t pressed);

#endif
