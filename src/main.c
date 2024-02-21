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
    start_color();

    init_pair(1, COLOR_CYAN, COLOR_BLACK); // Titres
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Utilisation CPU basse
    init_pair(3, COLOR_RED, COLOR_BLACK); // Utilisation CPU haute
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Utilisation mémoire élevée
}

int main() {
    ProcessInfo previous_processes[1024];
    int previous_num_processes = 0;

    init_screen();
    display_processes();
    while (1) { // Boucle infinie
        ProcessInfo current_processes[1024];
        int current_num_processes = get_processes(current_processes);

        // Compare l'état actuel à l'état précédent
        if (current_num_processes != previous_num_processes || memcmp(current_processes, previous_processes, sizeof(ProcessInfo) * current_num_processes) != 0) {
            // Si quelque chose a changé, rafraîchit l'affichage
            display_processes(current_processes, current_num_processes);

            // Met à jour l'état précédent
            memcpy(previous_processes, current_processes, sizeof(ProcessInfo) * current_num_processes);
            previous_num_processes = current_num_processes;
        }

        nodelay(stdscr, TRUE); // Ne pas attendre l'entrée de l'utilisateur
        int ch = getch();
        nodelay(stdscr, FALSE); // Retourner à l'attente de l'entrée de l'utilisateur

        if (ch == 'q') {
            break; // Quitter la boucle si 'q' est pressé
        } else if (ch == ' ') {
            // Forcer une actualisation si ESPACE est pressé
            display_processes(current_processes, current_num_processes);
            memcpy(previous_processes, current_processes, sizeof(ProcessInfo) * current_num_processes);
            previous_num_processes = current_num_processes;
        }

        sleep(1); // Pause de 5 secondes
    }
    endwin();
    return 0;
}

