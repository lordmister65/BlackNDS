#include <nds.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * BlackNDS hardware boot test.
 * ARM9/libnds only. No FAT access, DLDI calls, assets or RPG Maker data.
 * If the R4 stays on Loading, execution has not reached this screen.
 */
int main(void) {
    consoleDemoInit();

    iprintf("\x1b[2J\x1b[0;0H");
    iprintf("BLACKNDS R4 CHECK 0.3\n");
    iprintf("------------------------\n");
    iprintf("ARM9 started successfully\n");
    iprintf("libnds video: OK\n");
    iprintf("SD / DLDI: NOT REQUIRED\n");
    iprintf("\n");
    iprintf("Press A to test input\n");
    iprintf("Press B to reset counter\n");
    iprintf("START to toggle display\n");
    iprintf("\n");
    iprintf("If you can read this,\n");
    iprintf("homebrew boot succeeded.\n");

    unsigned frames = 0, a_presses = 0;
    bool show_frames = true;
    while (pmMainLoop()) {
        swiWaitForVBlank();
        scanKeys();
        const unsigned pressed = keysDown();
        ++frames;
        if (pressed & KEY_A) ++a_presses;
        if (pressed & KEY_B) { a_presses = 0; frames = 0; }
        if (pressed & KEY_START) show_frames = !show_frames;
        iprintf("\x1b[17;0H");
        iprintf("A presses: %-8lu\n", (unsigned long)a_presses);
        if (show_frames)
            iprintf("VBlanks:   %-8lu\n", (unsigned long)frames);
        else
            iprintf("VBlanks:   (hidden)\n");
    }
    return 0;
}
