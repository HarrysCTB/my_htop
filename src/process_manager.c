#include "../include/process_manager.h"

int compare_processes(const void* a, const void* b) {
    return ((ProcessInfo*)a)->pid - ((ProcessInfo*)b)->pid;
}

int compare_processes_by_pid(const void* a, const void* b) {
    return ((ProcessInfo*)a)->pid - ((ProcessInfo*)b)->pid;
}

int compare_processes_by_cpu(const void* a, const void* b) {
    return ((ProcessInfo*)a)->utime + ((ProcessInfo*)a)->stime - ((ProcessInfo*)b)->utime - ((ProcessInfo*)b)->stime;
}

int compare_processes_by_memory(const void* a, const void* b) {
    return ((ProcessInfo*)a)->rss - ((ProcessInfo*)b)->rss;
}

void sort_processes(ProcessInfo* processes, int num_processes, int (*compare)(const void*, const void*)) {
    qsort(processes, num_processes, sizeof(ProcessInfo), compare);
}

void get_process_info(ProcessInfo* process, char* pid) {
    char path[1054], cmdline[1054], stat[1054], status[1054], exe_path[1054];
    FILE *fp;

    // Lire le fichier cmdline
    snprintf(path, sizeof(path), "/proc/%s/cmdline", pid);
    if ((fp = fopen(path, "r")) != NULL) {
        if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
            for (char *p = cmdline; *p; ++p) {
                if (*p == '\0') *p = ' ';
            }
        }
        fclose(fp);
    }

    // Lire le fichier stat
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);
    if ((fp = fopen(path, "r")) != NULL) {
        if (fgets(stat, sizeof(stat), fp) != NULL) {
            // Extraire les informations nécessaires du fichier stat
            char *token = strtok(stat, " ");
            for (int i = 0; i < 13; ++i) token = strtok(NULL, " ");
            long utime = atol(token);
            token = strtok(NULL, " ");
            long stime = atol(token);
            for (int i = 0; i < 7; ++i) token = strtok(NULL, " ");
            long rss = atol(token);
            token = strtok(NULL, " ");
            char state = token[0];
            token = strtok(NULL, " ");
            long start_time = atol(token);

            // Stocker les informations sur le processus
            process->pid = atoi(pid);
            strcpy(process->cmdline, cmdline);
            process->utime = utime;
            process->stime = stime;
            process->rss = rss;
            process->state = state;
            process->start_time = start_time;
        }
        fclose(fp);
    }

    // Lire le fichier status
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    if ((fp = fopen(path, "r")) != NULL) {
        while (fgets(status, sizeof(status), fp) != NULL) {
            if (strncmp(status, "Uid:", 4) == 0) {
                process->uid = atoi(status + 5);
            } else if (strncmp(status, "Threads:", 8) == 0) {
                process->num_threads = atoi(status + 9);
            }
        }
        fclose(fp);
    }

    // Lire le lien exe
    snprintf(path, sizeof(path), "/proc/%s/exe", pid);
    memset(exe_path, 0, sizeof(exe_path));  // Initialise exe_path à zéro
    if (readlink(path, exe_path, sizeof(exe_path) - 1) != -1) {  // Laisse de la place pour '\0'
        strcpy(process->exe_path, exe_path);
    }
}

void display_process(ProcessInfo* process) {
    struct passwd *pw = getpwuid(process->uid);
    char *username = pw ? pw->pw_name : "unknown";

    // Choisissez la paire de couleurs en fonction de l'utilisation de la CPU
    int color_pair = process->cpu_usage > 50 ? 3 : 2;
    attron(COLOR_PAIR(color_pair));

    // Votre code d'affichage ici
    static int line = 3; // Démarrez l'affichage à partir de la 3ème ligne, par exemple
    mvprintw(line++, 0, "%5d %-20s %5ld %10ld %8s %3d %c %10ld %-20s",
            process->pid, process->cmdline, process->utime + process->stime,
            process->rss, username, process->num_threads, process->state,
            process->start_time, process->exe_path);


    attroff(COLOR_PAIR(color_pair));
}

void display_processes() {
    DIR *dir;
    struct dirent *ent;
    ProcessInfo processes[1024];
    int num_processes = 0;
    int max_y, max_x;

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // Titres
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Utilisation CPU basse
    init_pair(3, COLOR_RED, COLOR_BLACK); // Utilisation CPU haute
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    clear();
    getmaxyx(stdscr, max_y, max_x); // stdscr est la fenêtre standard de Ncurses

    attron(COLOR_PAIR(1));
    if (max_x) {}
    mvprintw(max_y - 3, 0, "Appuyez sur 1, 2 ou 3 pour trier par ID, utilisation CPU ou mémoire, puis appuyez sur ESPACE pour rafraîchir.");
    mvprintw(max_y - 2, 0, "Appuyez sur 'q' pour quitter.");

    mvprintw(0, 0, "%5s %-20s %5s %10s %8s %3s %5s %10s %-20s", "PID", "Nom du processus", "CPU", "Mémoire", "Utilisateur", "Thr", "Etat", "Démarrage", "Exe");
    mvprintw(2, 0, "------------------------------------------------------------------------------------------------------------------");
    attroff(COLOR_PAIR(1));

    if ((dir = opendir("/proc")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
 if (ent->d_type == DT_DIR && strspn(ent->d_name, "0123456789") == strlen(ent->d_name)) {
                get_process_info(&processes[num_processes], ent->d_name);
                ++num_processes;
            }
        }
        closedir(dir);
    }

    nodelay(stdscr, TRUE); // Ne pas attendre l'entrée de l'utilisateur
    int option = getch() - '0'; // Convertir le caractère en nombre
    nodelay(stdscr, FALSE); // Retourner à l'attente de l'entrée de l'utilisateur

    switch (option) {
        case 1:
            sort_processes(processes, num_processes, compare_processes_by_pid);
            break;
        case 2:
            sort_processes(processes, num_processes, compare_processes_by_cpu);
            break;
        case 3:
            sort_processes(processes, num_processes, compare_processes_by_memory);
            break;
        default:
            sort_processes(processes, num_processes, compare_processes_by_pid);
            break;
    }

    for (int i = 0; i < num_processes; ++i) {
        display_process(&processes[i]);
    }

    refresh();
}