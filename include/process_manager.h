#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

typedef struct {
    pid_t pid;
    char cmdline[1054];
    float cpu_usage;
    long utime;  // Temps d'exécution en mode utilisateur
    long stime;  // Temps d'exécution en mode système
    long rss;    // Taille de la mémoire résidente
    uid_t uid;   // ID de l'utilisateur propriétaire
    int num_threads;  // Nombre de threads
    char state;  // État du processus
    long start_time;  // Temps de démarrage
    char exe_path[1054];  // Chemin du fichier exécutable
} ProcessInfo;

void get_process_info(ProcessInfo* process, char* pid);
void sort_processes(ProcessInfo* processes, int num_processes);
void display_process(ProcessInfo* process);
void display_processes();

#endif
