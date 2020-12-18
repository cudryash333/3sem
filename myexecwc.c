#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

int me_quiet = 0;

int analyse(int argc, char **argv)
{
	optind = 3;
	int next_argument = 0;
	int option_index = 0;
	struct option long_options[] = {
		{"quiet", next_argument, 0, 'q'},
		{0, 0, 0, 0}
	};

	while (1) {
		next_argument =
		    getopt_long(argc, argv, "+q", long_options, &option_index);
		if (next_argument == -1)
			break;

		switch (next_argument) {
		case 'q':
			me_quiet = 1;
			break;

		case '?':
			break;

		default:
			perror("Unknown argument:");
			return -1;
		}
	}

	return 0;
}

void PrintWC(int pipefd)
{
	char buf;
	int maybe_word = 0;
	int words = 0;
	int bytes = 0;
	int lines = 0;

	while (read(pipefd, &buf, 1) > 0) {
		if (isalpha(buf) && words == 0) {
			words = 1;
		}
		if (maybe_word && !isspace(buf)) {
			words++;
			maybe_word = 0;
		}
		if (isspace(buf)) {
			maybe_word = 1;
		}
		if (!me_quiet) {
			putc(buf, stdout);
		}
		if (buf == '\n') {
			lines++;
		}
		bytes++;
	}
	printf("%d %d %d\n", lines, words, bytes);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		return 0;
	}
	analyse(argc, argv);
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
