#include "../include/process_manager.h"


/*
 * Initializes screen for display.
 * Use the ncurses library to initialize the screen,
 * disable line buffering, do not display keyboard inputs on screen,
 * and activate function keys.
 */
void init_screen() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
}

int main() {
    init_screen();
    display_processes();
    while (1) {
        int ch = getch();
        if (ch == 'q') break; // Quitter avec 'q'
        display_processes(); // Rafraîchir l'affichage pour toute autre touche
    }
    endwin();
    return 0;
}
