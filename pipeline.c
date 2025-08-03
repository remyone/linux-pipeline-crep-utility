#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define dt "\x1b[0m"
#define red "\x1b[31m"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf(red"Usage %s: filename word\n"dt, argv[0]);
        exit(-1);
    }


    int fd[2];
    pid_t child1, child2;

    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(-1);
    }

    if ((child1 = fork()) == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execl("/bin/cat", "cat", argv[1], NULL);
        perror("execl cat failed");
        exit(EXIT_FAILURE);
    }

    if ((child2 = fork()) == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        execl("/bin/grep", "grep", argv[2], NULL);
        perror("execl grep failed");
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}