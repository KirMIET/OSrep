#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s seed arraysize\n", argv[0]);
        return 1;
    }

    int seed = atoi(argv[1]);
    int array_size = atoi(argv[2]);

    if (seed <= 0 || array_size <= 0) {
        printf("seed and arraysize must be positive numbers\n");
        return 1;
    }

    pid_t pid = fork();  

    if (pid == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        execl("./sequential_min_max", "sequential_min_max", argv[1], argv[2], NULL);
        
        perror("execl failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);  
        
        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process terminated abnormally\n");
        }
    }

    return 0;
}