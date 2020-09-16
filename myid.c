#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

int main(int ac, char *av[]) {
	int uid = getuid();
	struct passwd* p = getpwuid(uid);
	char *name = p->pw_name;
	int gid = getgid();
	struct group* g = getgrgid(gid);
	char *grnm = g->gr_name;
	gid_t list[256];
	int n = getgroups(256, list);
	printf("uid=%d(%s) gid=%d(%s) группы=%d(%s)", uid, name, gid, grnm, gid, grnm);
	for (int i = 0; i < n - 1; i++) {
		struct group* gr = getgrgid(list[i]);
		char *grname = gr->gr_name;
		printf(",%d(%s)", list[i], grname);
	}
	printf("\n");
	return 0;
}

