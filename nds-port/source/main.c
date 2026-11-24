#include <nds.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
 * BlackNDS native RPG engine experiment, v0.4.
 * Generated demonstration map; original BLACK SOULS maps/assets NOT imported.
 * No filesystem, DLDI, sound, or save-game requirements.
 */
enum { MAP_W=32, MAP_H=24, TILE=16, VIEW_W=16, VIEW_H=12, MAX_HP=24 };
typedef enum { EXPLORE, DIALOG, MENU, BATTLE } GameMode;

static char map[MAP_H][MAP_W + 1];
static u16 *framebuffer;
static PrintConsole bottom;
static int px=3, py=3, facing=2, hp=MAX_HP, potions=2, coins=0;
static int enemy_hp=0, enemy_turn=0, move_cooldown=0;
static bool chest_open=false, draw_needed=true;
static GameMode mode=EXPLORE;
static char note[64]="Explore o mapa demonstrativo.";
static const int npc_x=13, npc_y=8, chest_x=23, chest_y=16;

static inline u16 color(int r, int g, int b) {
    return (u16)(RGB15(r,g,b) | BIT(15));
}
static void rect(int x,int y,int w,int h,u16 c) {
    if(x<0 || y<0 || x+w>256 || y+h>192) return;
    for(int j=y;j<y+h;j++)
        for(int i=x;i<x+w;i++) framebuffer[j*256+i]=c;
}
static void init_map(void) {
    for(int y=0;y<MAP_H;y++) {
        for(int x=0;x<MAP_W;x++) {
            char c='.';
            if(x==0||y==0||x==MAP_W-1||y==MAP_H-1) c='#';
            if((y==6 && x>=5 && x<=22 && x!=11 && x!=18) ||
               (x==19 && y>=9 && y<=20 && y!=13 && y!=18) ||
               (y==15 && x>=4 && x<=15 && x!=9)) c='#';
            if ((x>24 && y<7) || (x<6 && y>18)) c='~';
            map[y][x]=c;
        }
        map[y][MAP_W]=0;
    }
    map[npc_y][npc_x]='N';
    map[chest_y][chest_x]='C';
    map[12][9]='H'; /* healing fountain */
    map[20][25]='E'; /* roaming encounter placeholder */
}
static bool walkable(int x,int y) {
    return x>=0&&x<MAP_W&&y>=0&&y<MAP_H&&
       map[y][x]!='#'&&map[y][x]!='~'&&
       map[y][x]!='N'&&map[y][x]!='C';
}
static bool adjacent(int x,int y) {
    int dx=px-x,dy=py-y;
    return (dx==0&&(dy==1||dy==-1)) || (dy==0&&(dx==1||dx==-1));
}
static void render_tile(char t,int sx,int sy,int wx,int wy) {
    int x=sx*TILE,y=sy*TILE;
    bool alt=((wx+wy)&1)!=0;
    u16 floor=alt?color(9,13,11):color(10,15,12);
    rect(x,y,TILE,TILE,floor);
    rect(x+2,y+3,2,2,color(12,16,13));
    if(t=='#') {
        rect(x,y,TILE,TILE,color(8,8,10));
        rect(x+1,y+1,14,5,color(15,15,18));
        rect(x+1,y+7,14,7,color(11,11,14));
        rect(x+7,y+7,2,7,color(6,6,9));
    } else if(t=='~') {
        rect(x,y,TILE,TILE,color(3,8,17));
        rect(x+2,y+4,8,2,color(7,13,23));
        rect(x+6,y+11,8,2,color(7,13,23));
    } else if(t=='N') {
        rect(x+5,y+3,6,5,color(26,19,13));
        rect(x+4,y+8,8,7,color(8,13,25));
        rect(x+5,y+2,6,2,color(12,9,7));
    } else if(t=='C') {
        rect(x+2,y+6,12,9,chest_open?color(10,10,10):color(20,11,3));
        rect(x+3,y+5,10,3,color(26,19,7));
        rect(x+7,y+7,2,5,color(31,26,5));
    } else if(t=='H') {
        rect(x+3,y+3,10,10,color(12,12,13));
        rect(x+5,y+4,6,7,color(5,19,27));
    } else if(t=='E') {
        rect(x+4,y+3,8,10,color(23,5,7));
        rect(x+6,y+5,2,2,color(31,28,6));
        rect(x+10,y+5,2,2,color(31,28,6));
    }
}
static void render_world(void) {
    int cx=px-8,cy=py-6;
    if(cx<0)cx=0;
    if(cy<0)cy=0;
    if(cx>MAP_W-VIEW_W)cx=MAP_W-VIEW_W;
    if(cy>MAP_H-VIEW_H)cy=MAP_H-VIEW_H;
    for(int sy=0;sy<VIEW_H;sy++)
        for(int sx=0;sx<VIEW_W;sx++) {
            int wx=cx+sx,wy=cy+sy;
            render_tile(map[wy][wx],sx,sy,wx,wy);
        }
    int sx=(px-cx)*TILE,sy=(py-cy)*TILE;
    rect(sx+4,sy+3,8,5,color(27,18,13));
    rect(sx+3,sy+8,10,6,color(7,13,27));
    rect(sx+5,sy+1,6,3,color(15,10,7));
    if(facing==0) rect(sx+7,sy+1,2,2,color(31,28,18));
    else if(facing==1) rect(sx+7,sy+11,2,2,color(31,28,18));
    else if(facing==2) rect(sx+4,sy+6,2,2,color(31,28,18));
    else rect(sx+10,sy+6,2,2,color(31,28,18));
    rect(0,0,256,3,color(27,19,7));
    rect(0,189,256,3,color(27,19,7));
}
static void render_hud(void) {
    consoleSelect(&bottom);
    iprintf("\x1b[2J\x1b[0;0H");
    iprintf("BLACKNDS | ENGINE 0.4\n");
    iprintf("------------------------------\n");
    iprintf("HP %02d/%02d  Pocoes %d  Ouro %d\n",hp,MAX_HP,potions,coins);
    iprintf("Posicao: %02d,%02d\n\n",px,py);
    if(mode==EXPLORE) {
        iprintf("D-PAD  mover pelo mapa\n");
        iprintf("A      interagir / fonte\n");
        iprintf("START  inventario\n");
        iprintf("SELECT combate demonstrativo\n\n");
        iprintf("Azul: jogador | N: personagem\n");
        iprintf("Bau dourado: recompensa\n");
        iprintf("Vermelho: inimigo\n");
    } else if(mode==DIALOG) {
        iprintf("DIALOGO\n\n");
        iprintf("O viajante diz:\n");
        iprintf("Este mundo ainda e um teste.\n");
        iprintf("Os mapas originais virao\n");
        iprintf("em uma etapa de conversao.\n\n");
        iprintf("A/B fechar\n");
    } else if(mode==MENU) {
        iprintf("INVENTARIO\n\n");
        iprintf("Pocoes: %d\n",potions);
        iprintf("A - usar pocao (+10 HP)\n");
        iprintf("B/START - fechar\n");
    } else {
        iprintf("COMBATE POR TURNOS\n\n");
        iprintf("Inimigo HP: %02d\n",enemy_hp);
        iprintf("Turno: %d\n\n",enemy_turn);
        iprintf("A - atacar (4 dano)\n");
        iprintf("X - pocao (10 HP)\n");
        iprintf("B - fugir\n");
    }
    iprintf("\n------------------------------\n");
    iprintf("%-30.30s\n",note);
}
static void set_note(const char* s) {
    strncpy(note,s,sizeof(note)-1);
    note[sizeof(note)-1]=0;
    draw_needed=true;
}
static void start_battle(void) {
    enemy_hp=12;enemy_turn=0;mode=BATTLE;
    set_note("Encontro de teste!");
}
static void try_interact(void) {
    if(adjacent(npc_x,npc_y)) {
        mode=DIALOG;set_note("Personagem encontrado.");
    } else if(adjacent(chest_x,chest_y)) {
        if(!chest_open) {
            chest_open=true; coins+=10; potions++;
            set_note("Bau: +10 ouro, +1 pocao");
        } else set_note("O bau esta vazio.");
    } else if(map[py][px]=='H') {
        hp=MAX_HP;set_note("Fonte: HP restaurado!");
    } else set_note("Nada para interagir aqui.");
}
static void enemy_attack(void) {
    hp-=3;
    if(hp<=0) {
        hp=MAX_HP;px=3;py=3;mode=EXPLORE;
        set_note("Derrota! Retorno ao inicio.");
    } else set_note("Inimigo atacou: -3 HP");
}
static void update(unsigned held,unsigned down) {
    if(mode==DIALOG) {
        if(down&(KEY_A|KEY_B)){mode=EXPLORE;draw_needed=true;}
        return;
    }
    if(mode==MENU) {
        if(down&KEY_A) {
            if(potions>0 && hp<MAX_HP) {
                potions--;hp+=10;if(hp>MAX_HP)hp=MAX_HP;
                set_note("Pocao utilizada.");
            } else set_note("Nao e possivel usar pocao.");
        }
        if(down&(KEY_B|KEY_START)){mode=EXPLORE;draw_needed=true;}
        return;
    }
    if(mode==BATTLE) {
        if(down&KEY_B){mode=EXPLORE;set_note("Voce fugiu.");return;}
        if((down&KEY_X)&&potions>0&&hp<MAX_HP) {
            potions--;hp+=10;if(hp>MAX_HP)hp=MAX_HP;
            enemy_attack();draw_needed=true;return;
        }
        if(down&KEY_A) {
            enemy_hp-=4;enemy_turn++;
            if(enemy_hp<=0) {
                mode=EXPLORE;coins+=5;
                set_note("Vitoria! +5 ouro");
            } else enemy_attack();
            draw_needed=true;
        }
        return;
    }
    if(down&KEY_START){mode=MENU;draw_needed=true;return;}
    if(down&KEY_SELECT){start_battle();return;}
    if(down&KEY_A){try_interact();return;}
    unsigned directions=held&(KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT);
    if(!directions){move_cooldown=0;return;}
    unsigned fresh=down&(KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT);
    if(!fresh && ++move_cooldown<7)return;
    move_cooldown=0;
    int nx=px,ny=py;
    if(held&KEY_UP){ny--;facing=0;}
    else if(held&KEY_DOWN){ny++;facing=1;}
    else if(held&KEY_LEFT){nx--;facing=2;}
    else if(held&KEY_RIGHT){nx++;facing=3;}
    if(walkable(nx,ny)) {
        px=nx;py=ny;draw_needed=true;
        if(map[py][px]=='E')start_battle();
    }
}
int main(void) {
    lcdMainOnTop();
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    int bg=bgInit(2,BgType_Bmp16,BgSize_B16_256x256,0,0);
    framebuffer=(u16*)bgGetGfxPtr(bg);
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&bottom,3,BgType_Text4bpp,BgSize_T_256x256,31,0,false,true);
    consoleSelect(&bottom);
    init_map();
    while(pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        update(keysHeld(),keysDown());
        if(draw_needed) {
            render_world();
            render_hud();
            draw_needed=false;
        }
    }
    return 0;
}
