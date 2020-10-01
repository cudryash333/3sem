#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int ac, char *av[]) {
	int n = strtoul(av[1], NULL, 10);
	if (strcmp (av[2], "1") == 0) {	
		printf("parent %d\n", getpid());
		for (int i = 0; i < n; i++) {
			if (fork() == 0) {
				printf("child %d %d\n", getpid(), getppid());
				exit(0);
			}
		}
	}
	if (strcmp(av[2], "2") == 0) {
		if (fork() > 0) {
			printf("parent %d\n", getpid());
			exit(0);
		}
		for (int i = 1; i < n; i++) {
                	if (fork() > 0) {
                        	printf("child %d %d\n", getpid(), getppid());
				exit(0);
			}
                }
		printf("child %d %d\n", getpid(), getppid());
        }
	return 0;
}
