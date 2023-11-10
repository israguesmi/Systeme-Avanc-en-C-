#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <délai en secondes> <nombre d'envois>\n", argv[0]);
        exit(1);
    }

    int delai = atoi(argv[1]);
    int nb_envois = atoi(argv[2]);

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Code du fils
        close(pipe_fd[1]); // Ferme l'extrémité d'écriture du pipe

        for (int i = 0; i < nb_envois; i++) {
            char message[256];
            time_t temps;
            struct tm date;

            read(pipe_fd[0], message, sizeof(message));

            temps = time(NULL);
            date = *localtime(&temps);

            printf("Fils - Recu de mon pere : %s envoye a %s", message, asctime(&date));
            sleep(delai);
        }

        printf("Fils - je me termine a ");
        time_t temps;
        struct tm date;
        temps = time(NULL);
        date = *localtime(&temps);
        printf("%s", asctime(&date));
        close(pipe_fd[0]);
    } else {
        // Code du père
        close(pipe_fd[0]); // Ferme l'extrémité de lecture du pipe

        for (int i = 0; i < nb_envois; i++) {
            char message[256];
            snprintf(message, sizeof(message), "Message numero %d envoye a ", i);
            write(pipe_fd[1], message, sizeof(message));
            sleep(delai);
        }

        close(pipe_fd[1]); // Ferme l'extrémité d'écriture du pipe
        wait(NULL); // Attends la fin du fils

        printf("pere - je me termine en dernier a ");
        time_t temps;
        struct tm date;
        temps = time(NULL);
        date = *localtime(&temps);
        printf("%s", asctime(&date));
    }

    return 0;
}
