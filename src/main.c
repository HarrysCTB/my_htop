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

    init_pair(1, COLOR_CYAN, COLOR_BLACK); // Titres
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Utilisation CPU basse
    init_pair(3, COLOR_RED, COLOR_BLACK); // Utilisation CPU haute
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Utilisation mémoire élevée
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
