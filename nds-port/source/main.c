#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * BLACKNDS: demonstrador de controles e estados (sem mapas/assets do jogo).
 * O projeto VX Ace usa RGSS3: importação exige conversão de dados e engine nativa.
 */
enum { MAP_W = 24, MAP_H = 14, MOVE_REPEAT = 7 };
typedef enum { MODE_EXPLORE, MODE_DIALOG, MODE_MENU, MODE_BATTLE } Mode;

static const char *world[MAP_H] = {
    "########################",
    "#......................#",
    "#..#####...............#",
    "#......#...............#",
    "#......#......####.....#",
    "#......#...............#",
    "#...........N..........#",
    "#...........######.....#",
    "#......................#",
    "#..######..............#",
    "#......................#",
    "#............####......#",
    "#......................#",
    "########################"
};
static int player_x = 2, player_y = 2;
static int hp = 20;
static int enemy_hp = 12;
static int move_counter = 0;
static Mode mode = MODE_EXPLORE;
static bool redraw = true;

static bool walkable(int x, int y) {
    return x >= 0 && x < MAP_W && y >= 0 && y < MAP_H &&
           world[y][x] != '#' && world[y][x] != 'N';
}

static bool near_npc(void) {
    const int dx = player_x - 12, dy = player_y - 6;
    return (dx == 0 && (dy == 1 || dy == -1)) ||
           (dy == 0 && (dx == 1 || dx == -1));
}

static void draw(void) {
    iprintf("\x1b[2J\x1b[0;0H");
    iprintf("BLACKNDS - BASE TECNICA 02\n");
    iprintf("HP:%d   A:acao B:voltar\n", hp);
    iprintf("START:menu SELECT:batalha\n\n");
    for (int y = 0; y < MAP_H; ++y) {
        for (int x = 0; x < MAP_W; ++x) {
            iprintf("%c", (x == player_x && y == player_y) ? '@' : world[y][x]);
        }
        iprintf("\n");
    }
    iprintf("\n");
    switch (mode) {
        case MODE_EXPLORE:
            iprintf("@ jogador  N personagem\n");
            iprintf("Direcional: andar\n");
            break;
        case MODE_DIALOG:
            iprintf("NPC: Bem-vindo, viajante!\n");
            iprintf("Esta e uma fala demonstrativa.\n");
            iprintf("A ou B: fechar\n");
            break;
        case MODE_MENU:
            iprintf("MENU | HP: %d/20\n", hp);
            iprintf("A: recuperar HP\n");
            iprintf("B ou START: fechar\n");
            break;
        case MODE_BATTLE:
            iprintf("BATALHA DE TESTE | Inimigo:%d\n", enemy_hp);
            iprintf("A: atacar   B: fugir\n");
            break;
    }
}

static void handle_explore(unsigned held, unsigned pressed) {
    if (pressed & KEY_START) { mode = MODE_MENU; redraw = true; return; }
    if (pressed & KEY_SELECT) { enemy_hp = 12; mode = MODE_BATTLE; redraw = true; return; }
    if ((pressed & KEY_A) && near_npc()) { mode = MODE_DIALOG; redraw = true; return; }

    unsigned directions = held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
    if (!directions) { move_counter = 0; return; }
    unsigned fresh = pressed & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
    if (!fresh && ++move_counter < MOVE_REPEAT) return;
    move_counter = 0;

    int nx = player_x, ny = player_y;
    if (held & KEY_UP) --ny;
    else if (held & KEY_DOWN) ++ny;
    else if (held & KEY_LEFT) --nx;
    else if (held & KEY_RIGHT) ++nx;
    if (walkable(nx, ny)) { player_x = nx; player_y = ny; redraw = true; }
}

int main(void) {
    consoleDemoInit();
    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        const unsigned held = keysHeld(), pressed = keysDown();
        if (mode == MODE_EXPLORE) {
            handle_explore(held, pressed);
        } else if (mode == MODE_DIALOG) {
            if (pressed & (KEY_A | KEY_B)) { mode = MODE_EXPLORE; redraw = true; }
        } else if (mode == MODE_MENU) {
            if (pressed & KEY_A) { hp = 20; redraw = true; }
            if (pressed & (KEY_B | KEY_START)) { mode = MODE_EXPLORE; redraw = true; }
        } else if (mode == MODE_BATTLE) {
            if (pressed & KEY_A) {
                enemy_hp -= 4;
                if (enemy_hp <= 0) mode = MODE_EXPLORE;
                redraw = true;
            } else if (pressed & KEY_B) { mode = MODE_EXPLORE; redraw = true; }
        }
        if (redraw) { draw(); redraw = false; }
    }
    return 0;
}
