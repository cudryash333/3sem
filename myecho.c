#include <stdio.h>
#include <string.h>

int main(int ac, char *av[]) {
	int Newline = 1; 
	for (int i = 1; i < ac; i++) {
		if ((strcmp(av[i], "-n") != 0 ) || (i != 1)) {
			printf ("%s ", av[i]);
		} else {
			Newline = 0;
		}
	}
	if (Newline == 1) {
		printf("\n");
	}
	return 0;
}
