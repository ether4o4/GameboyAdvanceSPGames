#include <assert.h>
#include <stdio.h>
#include "../source/game_model.h"

static void start(Game *g) {
    game_reset(g);
    game_update(g, 0, GAME_START);
    assert(g->state == GAME_PLAYING);
}

int main(void) {
    Game g;
    int initial_x, initial_y, i;

    start(&g);
    initial_x = g.x;
    for (i = 0; i < 24; i++) game_update(&g, GAME_RIGHT, 0);
    assert(g.x > initial_x);

    start(&g);
    initial_y = g.y;
    game_update(&g, 0, GAME_JUMP);
    assert(g.y < initial_y);
    assert(g.vy < 0);
    for (i = 0; i < 90; i++) game_update(&g, 0, 0);
    assert(g.grounded);

    start(&g);
    g.x = 198 << 8;
    g.y = 120 << 8;
    g.grounded = 1;
    game_update(&g, 0, 0);
    assert(g.state == GAME_WIPEOUT);
    game_update(&g, 0, GAME_START);
    assert(g.state == GAME_PLAYING && (g.x >> 8) == 24);

    start(&g);
    g.x = 1780 << 8;
    game_update(&g, 0, 0);
    assert(g.state == GAME_FINISHED);
    game_update(&g, 0, GAME_START);
    assert(g.state == GAME_PLAYING && (g.x >> 8) == 24);

    puts("Trash Lid Dash model smoke tests passed");
    return 0;
}
