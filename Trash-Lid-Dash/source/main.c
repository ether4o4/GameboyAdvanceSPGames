#include <gba.h>
#include "game_model.h"

#define W 240
#define H 160
#define COLOR(r,g,b) RGB5(r,g,b)

static Game game;
static u16 *const screen = (u16 *)MODE3_FB;

static void rect(int x, int y, int w, int h, u16 color) {
    int x2 = x + w, y2 = y + h, row, col;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x2 > W) x2 = W;
    if (y2 > H) y2 = H;
    for (row = y; row < y2; row++) {
        u16 *pixel = screen + row * W + x;
        for (col = x; col < x2; col++) *pixel++ = color;
    }
}

static void circle(int cx, int cy, int radius, u16 color) {
    int y, rr = radius * radius;
    for (y = -radius; y <= radius; y++) {
        int span = 0;
        while ((span + 1) * (span + 1) + y * y <= rr) span++;
        rect(cx - span, cy + y, span * 2 + 1, 1, color);
    }
}

static void ellipse(int cx, int cy, int rx, int ry, u16 color) {
    int y;
    for (y = -ry; y <= ry; y++) {
        int span = rx * rx * (ry * ry - y * y) / (ry * ry);
        int x = 0;
        while ((x + 1) * (x + 1) <= span) x++;
        rect(cx - x, cy + y, x * 2 + 1, 1, color);
    }
}

static void line(int x0, int y0, int x1, int y1, u16 color) {
    int dx = x1 - x0, dy = y1 - y0;
    int steps = dx < 0 ? -dx : dx;
    int i;
    if ((dy < 0 ? -dy : dy) > steps) steps = dy < 0 ? -dy : dy;
    if (!steps) { rect(x0, y0, 1, 1, color); return; }
    for (i = 0; i <= steps; i++) rect(x0 + dx * i / steps, y0 + dy * i / steps, 1, 1, color);
}

/* Compact original 3x5 bitmap alphabet: A-Z, 0-9, colon, dash and space. */
static const u8 font[39][5] = {
    {2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},{7,4,6,4,7},
    {7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},{7,2,2,2,7},{1,1,1,5,2},
    {5,5,6,5,5},{4,4,4,4,7},{5,7,7,5,5},{6,5,5,5,5},{2,5,5,5,2},
    {6,5,6,4,4},{2,5,5,3,1},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},
    {5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},{5,5,2,2,2},
    {7,1,2,4,7},{2,5,5,5,2},{2,6,2,2,7},{6,1,2,4,7},{6,1,2,1,6},
    {5,5,7,1,1},{7,4,6,1,6},{3,4,6,5,2},{7,1,2,2,2},{2,5,2,5,2},
    {2,5,3,1,6},{2,5,3,5,2},{0,2,0,2,0},{0,0,7,0,0}
};

static int glyph_index(char ch) {
    if (ch >= 'A' && ch <= 'Z') return ch - 'A';
    if (ch >= '0' && ch <= '9') return 26 + ch - '0';
    if (ch == ':') return 36;
    if (ch == '-') return 37;
    return 38;
}

static void text(const char *value, int x, int y, u16 color, int scale) {
    while (*value) {
        const u8 *rows = font[glyph_index(*value++)];
        int row, col;
        for (row = 0; row < 5; row++) for (col = 0; col < 3; col++)
            if (rows[row] & (1 << (2 - col))) rect(x + col * scale, y + row * scale, scale, scale, color);
        x += 4 * scale;
    }
}

static void draw_background(int camera) {
    int y, i;
    for (y = 0; y < 92; y += 8) {
        int band = y / 8;
        u16 c = COLOR(22 - band, 24 - band / 2, 30 - band);
        rect(0, y, W, 8, c);
    }
    circle(204, 31, 13, COLOR(31, 25, 13));
    circle(204, 31, 9, COLOR(31, 29, 20));
    rect(0, 92, W, 36, COLOR(10, 13, 19));
    for (i = 0; i < 5; i++) {
        int bx = (i * 74 - camera / 3) % 370;
        if (bx < -55) bx += 370;
        rect(bx, 74 + (i % 2) * 8, 38, 52, COLOR(10 + i, 13 + i, 20 + i));
        rect(bx + 7, 82 + (i % 2) * 8, 5, 6, COLOR(27, 20, 10));
        rect(bx + 21, 82 + (i % 2) * 8, 5, 6, COLOR(27, 20, 10));
        rect(bx + 7, 97 + (i % 2) * 8, 5, 6, COLOR(27, 20, 10));
    }
    rect(0, 128, W, 32, COLOR(5, 10, 12));
}

static void draw_platform(int world_x, int y, int width, int camera) {
    int x = world_x - camera;
    if (x > W || x + width < 0) return;
    rect(x, y, width, H - y, COLOR(14, 17, 18));
    rect(x, y, width, 3, COLOR(7, 23, 17));
    rect(x, y + 3, width, 2, COLOR(17, 22, 18));
    for (int mark = x + 8; mark < x + width; mark += 26)
        rect(mark, y + 10 + (mark % 3) * 5, 9, 1, COLOR(10, 12, 12));
}

static void draw_hazard(int world_x, int y, int camera, int variant) {
    int x = world_x - camera;
    if (x < -30 || x > W + 30) return;
    if (variant & 1) {
        ellipse(x + 10, y + 10, 11, 9, COLOR(6, 8, 9));
        ellipse(x + 10, y + 8, 9, 7, COLOR(18, 22, 22));
        line(x + 4, y + 8, x + 16, y + 8, COLOR(24, 26, 23));
    } else {
        rect(x + 3, y + 4, 15, 10, COLOR(19, 13, 9));
        rect(x + 1, y + 2, 19, 4, COLOR(24, 17, 11));
        line(x + 5, y + 6, x + 8, y + 12, COLOR(26, 21, 14));
        line(x + 14, y + 6, x + 11, y + 12, COLOR(26, 21, 14));
    }
}

static void draw_hero(void) {
    int x = (game.x >> 8) - game.camera;
    int y = game.y >> 8;
    ellipse(x + 6, y + 17, 15, 4, COLOR(25, 19, 6));
    ellipse(x + 6, y + 16, 13, 3, COLOR(30, 24, 10));
    line(x - 3, y + 16, x + 15, y + 16, COLOR(31, 28, 15));
    rect(x + 1, y + 5, 10, 9, COLOR(4, 15, 21));
    rect(x + 2, y + 6, 8, 7, COLOR(5, 24, 27));
    circle(x + 6, y + 3, 5, COLOR(23, 16, 10));
    rect(x + 2, y + 1, 8, 2, COLOR(4, 8, 10));
    rect(x + 9, y + 3, 2, 1, COLOR(31, 28, 20));
    line(x + 2, y + 12, x, y + 16, COLOR(23, 16, 10));
    line(x + 10, y + 12, x + 13, y + 15, COLOR(23, 16, 10));
    line(x + 4, y + 13, x + 2, y + 16, COLOR(27, 12, 10));
    line(x + 8, y + 13, x + 10, y + 16, COLOR(27, 12, 10));
}

static void draw_game(void) {
    static const int px[] = {0,238,274,362,390,516,542,642,676,848,874,978,1012,1216,1250,1356,1390,1580,1610,1848};
    static const int py[] = {136,136,126,126,136,136,116,116,136,136,124,124,136,136,112,112,136,136,122,122};
    static const int hx[] = {204,430,724,924,1080,1298,1464,1660};
    static const int hy[] = {122,122,122,110,122,96,122,108};
    int i;
    draw_background(game.camera);
    for (i = 0; i < 10; i++) draw_platform(px[i * 2], py[i * 2], px[i * 2 + 1] - px[i * 2], game.camera);
    for (i = 0; i < 8; i++) draw_hazard(hx[i], hy[i], game.camera, i);
    draw_hero();
    rect(7, 7, 72, 17, COLOR(1, 4, 5));
    text("DIST", 12, 12, COLOR(25, 27, 22), 1);
    {
        int value = game.distance / 10;
        char digits[5];
        int d;
        for (d = 3; d >= 0; d--) { digits[d] = (char)('0' + value % 10); value /= 10; }
        digits[4] = '\0';
        text(digits, 43, 12, COLOR(31, 25, 10), 1);
    }
    text("A JUMP", 160, 13, COLOR(20, 24, 19), 1);
    if (game.state == GAME_WIPEOUT || game.state == GAME_FINISHED) {
        rect(29, 57, 182, 48, COLOR(1, 4, 5));
        text(game.state == GAME_FINISHED ? "NICE RUN" : "WIPEOUT", 71, 66,
             game.state == GAME_FINISHED ? COLOR(29, 25, 9) : COLOR(29, 13, 10), 2);
        text("START RETRY", 73, 90, COLOR(24, 25, 20), 1);
    }
}

static void draw_title(void) {
    draw_background(0);
    ellipse(120, 111, 59, 12, COLOR(28, 21, 7));
    ellipse(120, 108, 54, 9, COLOR(30, 25, 11));
    circle(120, 81, 18, COLOR(4, 15, 21));
    circle(120, 59, 12, COLOR(23, 16, 10));
    rect(108, 52, 24, 5, COLOR(4, 8, 10));
    rect(111, 67, 18, 4, COLOR(10, 23, 22));
    text("TRASH LID DASH", 55, 18, COLOR(31, 27, 17), 2);
    text("A TO JUMP", 88, 133, COLOR(26, 23, 15), 1);
    text("START GO", 94, 143, COLOR(26, 23, 15), 1);
}

static uint16_t translate_keys(u16 keys) {
    uint16_t out = 0;
    if (keys & KEY_LEFT) out |= GAME_LEFT;
    if (keys & KEY_RIGHT) out |= GAME_RIGHT;
    if (keys & (KEY_A | KEY_B)) out |= GAME_JUMP;
    if (keys & KEY_START) out |= GAME_START;
    if (keys & KEY_SELECT) out |= GAME_SELECT;
    return out;
}

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    game_reset(&game);
    for (;;) {
        VBlankIntrWait();
        scanKeys();
        game_update(&game, translate_keys(keysHeld()), translate_keys(keysDown()));
        if (game.state == GAME_TITLE) draw_title();
        else draw_game();
    }
}
