#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#define LEN 1024

void PrintWC(int pipefd)
{
        char buf;
        size_t bytes = 0;
	size_t error_num = errno;
	char error_buf[LEN];
        strerror_r (error_num, error_buf, LEN);
        while (read(pipefd, &buf, 1) > 0) {
                bytes++;
        }
        printf("Stdout: %ld bytes\n", bytes);
	printf("Stderr: %ld bytes\n", strlen(error_buf));
}

int main(int argc, char *argv[])
{
        if (argc < 2) {
                return 0;
        }
        int pipefd[2];
        if (pipe(pipefd) == -1) {
		perror("pipe");
        }
        if (fork() == 0) {
                close(pipefd[0]);
                dup2(pipefd[1], 1);
                execvp(argv[2], argv + 2);

                perror("execvp error");
                exit(EXIT_FAILURE);
        }
        wait(NULL);
        close(pipefd[1]);
        PrintWC(pipefd[0]);
        close(pipefd[0]);
        return 0;
}

