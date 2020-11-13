#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int ac, char *av[]) {
	char *arguments[ac];
	struct timespec tp1, tp2;
	for (int i = 0; i < ac - 1; i++) {
		arguments[i] = av[i + 1];
	}
	clock_gettime(CLOCK_REALTIME, &tp1);
        fork();
	execvp(av[0], arguments);
	wait(0);
	clock_gettime(CLOCK_REALTIME, &tp2);
	long ex_time = 1000000000 * (tp2.tv_sec - tp1.tv_sec) + tp2.tv_nsec - tp1.tv_nsec;
	printf("Execution time: %ld nsec\n", ex_time);	
        return 0;
}
