#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

// Protótipo técnico. O cenário abaixo NÃO é um mapa extraído do BLACK SOULS.
static const char *level[] = {
    "########################",
    "#......................#",
    "#..#####...............#",
    "#......#...............#",
    "#......#......####.....#",
    "#......#...............#",
    "#......................#",
    "#...........######.....#",
    "#......................#",
    "#..######..............#",
    "#......................#",
    "#............####......#",
    "#......................#",
    "########################"
};
enum { MAP_W = 24, MAP_H = 14 };
static int player_x = 2, player_y = 2;

static bool can_move(int x, int y) {
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H) return false;
    return level[y][x] != '#';
}

static void draw_scene(void) {
    iprintf("\x1b[2J\x1b[0;0H");
    iprintf("BLACKNDS - PROTOTIPO 01\n");
    iprintf("Direcional: mover\n");
    iprintf("START: voltar ao inicio\n\n");
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            iprintf("%c", (x == player_x && y == player_y) ? '@' : level[y][x]);
        }
        iprintf("\n");
    }
    iprintf("\n@ jogador | # parede\n");
    iprintf("Teste tecnico, sem assets RPG Maker\n");
}

int main(void) {
    consoleDemoInit();
    draw_scene();
    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        const unsigned pressed = keysDown();
        int dx = 0, dy = 0;
        if (pressed & KEY_LEFT) dx = -1;
        else if (pressed & KEY_RIGHT) dx = 1;
        else if (pressed & KEY_UP) dy = -1;
        else if (pressed & KEY_DOWN) dy = 1;
        if (pressed & KEY_START) {
            player_x = 2;
            player_y = 2;
            draw_scene();
        } else if ((dx || dy) && can_move(player_x + dx, player_y + dy)) {
            player_x += dx;
            player_y += dy;
            draw_scene();
        }
    }
    return 0;
}
