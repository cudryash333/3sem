#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h> 
#include <stdlib.h>

int N = 4096;

int my_write(int fd, void* buf, int count) {
	int succ_write = 0;
    	while (succ_write < count) {
		int status = write(fd, buf + succ_write, count - succ_write);
		if (status < 1) {
			perror("write fail");
			return -1;
		}
		succ_write += status;
	}
	return 0;
}

int read_file(int fd, void** buf, int* cur_buf_size) {
	int succ_read = 0;
	if (*cur_buf_size < N) {
		*buf = (void*)realloc(*buf, N);
		*cur_buf_size = N;
      		if (buf == NULL) {
			perror("problems with realloc");
			return -1;
       		}
	}
	while (1) {
		if (succ_read != 0) {
			*buf = (void*)realloc(*buf, *cur_buf_size + N);
			*cur_buf_size += N;
			if (buf == NULL) {
		       		perror("problems with realloc");
				return -1;
			}
		}
		int status = read(fd, *buf + succ_read, N);
		if (status < 1) {
			if (status == 0 && errno == 0) 
				break;
			perror("read fail");
			return -1;
		}
		succ_read += status;
	}
       	return succ_read;
}

int my_read(int fd, void* buf, int count) {
	int succ_read = 0;
       	while (1) {
		int status = read(fd, buf, count);
		if (status < 1) {                       
			if (status == 0 && errno == 0) 
				break;
			perror("read fail");
			return -1;
	       	}
		succ_read += status;
		break;
       	} 
	return succ_read;
}

int main(int ac, char* av[]) {
	int fdin = 0;
	int fdout = 1;
	char str[N];
	void* buf = NULL;
	int cur_buf_size = 0;
	int count;
	if (av[1]) {
		for (int i = 1; i < ac; i++) {
			int cur_fd = open(av[i], O_RDWR);
		       	if (cur_fd >= 0) {
				if ((count = read_file(cur_fd, &buf, &cur_buf_size)) < 0) {
					perror("read fail");
					return -1;
				}
				close(cur_fd);
				if (my_write(fdout, buf, count) < 0) {
					perror("write fail");
					return -1;
				}
				close(fdout);
			} else {
				perror("open fail");
				return -1;
			}	
		}
	} else {		
		while (1) {
		       	if ((count = my_read(fdin, str, N)) < 0) {
				perror("read fail");
				return -1;
			}
			if (my_write(fdout, str, count) < 0){
				perror("write fail");
				return -1;
			}
		}
	}
	free(buf);
	return 0;
}
