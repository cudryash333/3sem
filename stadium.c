#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#define RUNNER_CAME (N + 2)
#define RUN_IS_END  (N + 1)

void judge(int N, int msg_key);
void runner(int i, int N, int msg_key);

struct msgbuf {
	long type;
};

int main(int ac, char *av[])
{
	if (ac < 2) {
		return 0;
	}
	char print_buf[BUFSIZ];
	setvbuf(stdout, print_buf, _IOLBF, BUFSIZ);
	int N = strtoul(av[1], NULL, 10);
	if (N < 1) {
		return 0;
	}
	int msg_key = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0700);
	if (msg_key < 0) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	// Создаем процесс "Судья"
	pid_t jpid = fork();
	if (jpid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (jpid == 0) {
		judge(N, msg_key);
		return 0;
	}
	// Создаем N процессов "бегун"
	for (int i = 1; i <= N; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0) {
			runner(i, N, msg_key);
			return 0;
		}
	}
	wait(NULL);
	// Эстафета завершилась, удаляем всё
	msgctl(msg_key, IPC_RMID, NULL);
	return 0;
}

void judge(int N, int msg_key)
{
	struct timespec tp1, tp2;
	struct msgbuf msg;
	printf("Judge started working\n");
	// Ждёт всех бегунов
	for (int i = 0; i < N; i++) {
		if (msgrcv(msg_key, &msg, sizeof(msg), RUNNER_CAME, 0) < 0) {
			perror("msgrcv");
			exit(EXIT_FAILURE);
		}
	}
	// Судья дает бегуну 1 команду "Старт"
	printf("Start!\n");
	clock_gettime(CLOCK_REALTIME, &tp1);	/// Время начала забега
	for (int i = 0; i < N; i++) {
		// Бегун i начинает бежать   
		msg.type = i + 1;
		if (msgsnd(msg_key, &msg, sizeof(msg), 0) < 0) {
			perror("msgrcv");
			exit(EXIT_FAILURE);
		}
		// Бегун i останавливается
		if (msgrcv(msg_key, &msg, sizeof(msg), i + 1, 0) < 0) {
			perror("msgrcv");
			exit(EXIT_FAILURE);
		}
	}

	// Конец забега
	printf("Finish!\n");

	clock_gettime(CLOCK_REALTIME, &tp2);	// Время конца забега
	printf("Time of running %ld mqsec\n",
	       1000000 * (tp2.tv_sec - tp1.tv_sec) + (tp2.tv_nsec -
						      tp1.tv_nsec) / 1000);
	// Бегуны расходятся
	for (int i = 0; i < N; i++) {
		msg.type = RUN_IS_END;
		if (msgsnd(msg_key, &msg, sizeof(msg), 0) < 0) {
			perror("msgrcv");
			exit(EXIT_FAILURE);
		}
	}
	// Судья покидает стадион
	printf("Judge went home\n");
}

void runner(int i, int N, int msg_key)
{
	struct msgbuf msg;
	// Приходят на стадион
	msg.type = RUNNER_CAME;
	printf("Runner %d came\n", i);
	if (msgsnd(msg_key, &msg, sizeof(msg), 0) < 0) {
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
	// Получает от судьи сообщение о начале забега
	if (msgrcv(msg_key, &msg, sizeof(msg), i, 0) < 0) {
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}

	printf("Runner %d runned up\n", i);
	msg.type = i;

	// Передает судье сообщение об остановке
	if (msgsnd(msg_key, &msg, sizeof(msg), 0) < 0) {
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
	// Получает от судьи сообщение о завершении забега
	if (msgrcv(msg_key, &msg, sizeof(msg), RUN_IS_END, 0) < 0) {
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}
	// Бегуны расходятся
	printf("Runner %d went home\n", i);
}
