#include "game_model.h"

#define FP(n) ((int32_t)(n) << 8)
#define PLAYER_W 12
#define PLAYER_H 16
#define LEVEL_END 1780

typedef struct Rect { int x, y, w, h; } Rect;

static const Rect platforms[] = {
    {0,    136, 238, 24}, {274, 126, 88, 34}, {390, 136, 126, 24},
    {542, 116, 100, 44}, {676, 136, 172, 24}, {874, 124, 104, 36},
    {1012, 136, 204, 24}, {1250, 112, 106, 48}, {1390, 136, 190, 24},
    {1610, 122, 238, 38}
};

static const Rect hazards[] = {
    {204, 122, 20, 14}, {430, 122, 22, 14}, {724, 122, 24, 14},
    {924, 110, 20, 14}, {1080, 122, 24, 14}, {1298, 96, 22, 14},
    {1464, 122, 25, 14}, {1660, 108, 22, 14}
};

static int overlaps(int ax, int ay, int aw, int ah, const Rect *b) {
    return ax < b->x + b->w && ax + aw > b->x &&
           ay < b->y + b->h && ay + ah > b->y;
}

static void begin_run(Game *game) {
    game->x = FP(24);
    game->y = FP(120);
    game->vx = 0;
    game->vy = 0;
    game->camera = 0;
    game->distance = 0;
    game->grounded = 1;
    game->coyote_frames = 7;
    game->jump_buffer_frames = 0;
    game->state = GAME_PLAYING;
}

void game_reset(Game *game) {
    if (!game) return;
    begin_run(game);
    game->state = GAME_TITLE;
}

void game_update(Game *game, uint16_t held, uint16_t pressed) {
    int old_y, next_x, next_y, old_feet, next_feet, px, py;
    int landed = 0;
    unsigned i;
    if (!game) return;

    if (game->state == GAME_TITLE) {
        if (pressed & (GAME_START | GAME_JUMP)) begin_run(game);
        return;
    }
    if (game->state == GAME_WIPEOUT || game->state == GAME_FINISHED) {
        if (pressed & (GAME_START | GAME_SELECT)) begin_run(game);
        return;
    }
    if (pressed & GAME_SELECT) {
        begin_run(game);
        return;
    }
    if (pressed & GAME_JUMP) game->jump_buffer_frames = 7;

    if (held & GAME_LEFT) game->vx -= 64;
    if (held & GAME_RIGHT) game->vx += 64;
    if (!(held & (GAME_LEFT | GAME_RIGHT))) {
        if (game->vx > 0) game->vx = game->vx > 48 ? game->vx - 48 : 0;
        if (game->vx < 0) game->vx = game->vx < -48 ? game->vx + 48 : 0;
    }
    if (game->vx > 640) game->vx = 640;
    if (game->vx < -640) game->vx = -640;

    if (game->grounded) game->coyote_frames = 7;
    else if (game->coyote_frames > 0) game->coyote_frames--;
    if (game->jump_buffer_frames > 0) game->jump_buffer_frames--;
    if (game->jump_buffer_frames && game->coyote_frames) {
        game->vy = -1120;
        game->grounded = 0;
        game->coyote_frames = 0;
        game->jump_buffer_frames = 0;
    }

    old_y = game->y >> 8;
    next_x = (game->x + game->vx) >> 8;
    if (next_x < 0) { next_x = 0; game->vx = 0; }
    game->x = FP(next_x);

    old_feet = old_y + PLAYER_H;
    game->vy += 40;
    if (game->vy > 1280) game->vy = 1280;
    next_y = (game->y + game->vy) >> 8;
    next_feet = next_y + PLAYER_H;
    if (game->vy >= 0) {
        for (i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
            const Rect *p = &platforms[i];
            if (old_feet <= p->y && next_feet >= p->y &&
                next_x + PLAYER_W > p->x && next_x < p->x + p->w) {
                next_y = p->y - PLAYER_H;
                game->vy = 0;
                game->grounded = 1;
                landed = 1;
                break;
            }
        }
    }
    if (!landed) game->grounded = 0;
    game->y = FP(next_y);
    px = game->x >> 8;
    py = game->y >> 8;

    for (i = 0; i < sizeof(hazards) / sizeof(hazards[0]); i++) {
        if (overlaps(px, py, PLAYER_W, PLAYER_H, &hazards[i])) {
            game->state = GAME_WIPEOUT;
            return;
        }
    }
    if (py > 176) {
        game->state = GAME_WIPEOUT;
        return;
    }
    if (px >= LEVEL_END) {
        game->state = GAME_FINISHED;
        return;
    }
    game->distance = px;
    game->camera = px > 78 ? px - 78 : 0;
}
