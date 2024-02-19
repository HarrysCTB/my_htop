#include <ncurses.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

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

/*
 * Displays processes on the screen.
 * Currently, this function cleans the screen and displays a static message.
 * In the future, it should be modified to read and display actual processes.
 */
void display_processes() {
    DIR *dir;
    struct dirent *ent;
    int y = 1;

    start_color();  // Initialise le système de couleur
    init_pair(1, COLOR_CYAN, COLOR_BLACK);  // Définit une paire de couleurs

    clear();
    attron(COLOR_PAIR(1));  // Active la couleur
    mvprintw(0, 0, "PID\tNom du processus");
    mvprintw(2, 0, "\n--------------------------------");
    attroff(COLOR_PAIR(1));  // Désactive la couleur
    if ((dir = opendir("/proc")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_DIR && strspn(ent->d_name, "0123456789") == strlen(ent->d_name)) {
                char path[1054], cmdline[1054];
                FILE *fp;

                // Assurez-vous que le chemin ne dépassera pas la taille du buffer
                int needed = snprintf(NULL, 0, "/proc/%s/cmdline", ent->d_name);
                if (needed >= (int)sizeof(path)) {
                    // Gestion d'erreur : chemin trop long
                    continue; // Passer à l'entrée suivante ou gérer l'erreur comme nécessaire
                }
                snprintf(path, sizeof(path), "/proc/%s/cmdline", ent->d_name);

                if ((fp = fopen(path, "r")) != NULL) {
                    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
                        // Traitement de cmdline
                        for (char *p = cmdline; *p; ++p) {
                            if (*p == '\0') *p = ' ';
                        }
                        mvprintw(y++, 0, "%s\t%s", ent->d_name, cmdline);
                    }
                    fclose(fp);
                }
            }
        }
        closedir(dir);
    }
    refresh();
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
