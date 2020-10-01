#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h> 

int copy(int fdin, int fdout) {
	int N = 4096;
	void* buf[N];
	int rd = read(fdin, buf, N);
	int wr = write(fdout, buf, N);
	if (rd >= 0) {
		while(rd) {
        		if (wr >= 0) {
				N = N - wr;
				for (int i = 0; i < N; i++) {
					buf[i] = buf[i + wr];
				}
			} else {	
				perror("write_fail:");
			}
		}
        } else {
		perror("read_fail:");
	}
	return wr;
}

int main(int ac, char* av[]) {
	if (av[1]) {
		for (int i = 1; i < ac; i++) {
			if (open(av[i], O_RDWR) >= 0) {
				copy(0, 1);
				close(1);
			} else {
				perror("Not such file or directory");
			}	
		}
	} else {		
		copy(0, 1);
	}
	return 0;
}
