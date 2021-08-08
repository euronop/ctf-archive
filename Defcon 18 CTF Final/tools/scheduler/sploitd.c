#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sqlite3.h>

#include "db.h"
#include "log.h"

#define SLEEP_INTERVAL 1

char *db_file = DB_FILE;

void 
spawn_worker(void) {

	pid_t pid;

	pid = fork();

	if (pid < 0) {
		perror("master worker fork() failed");
		return;

	} else if (pid == 0) {
		sploit_worker();
		_exit(EXIT_SUCCESS);
	}

	/* parent */
	return;
}


void 
spawn_killer(void) {
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		perror("killer fork() failed");
		return;

	} else if (pid == 0) {
		killer();
		_exit(EXIT_SUCCESS);
	}

	/* parent */
	return;
}

int 
main(int argc, char *argv[]) {

	int i;

	/* prevent zombies */
	signal(SIGCHLD, SIG_IGN);


	if (argc > 3) {
		printf("usage: %s [db-file]\n", argv[0]);
	}

	if (argc == 2) {
		db_file = argv[1];
	}

	srand(time(NULL));

	while(1) {

// 		if (++i >= 99) {
// 			i = 0;
// 			printf("-------------------------\n");
// 			printf("KILLING\n");
// 			printf("-------------------------\n");
			spawn_killer();
// 		}

		spawn_worker();

		sleep(SLEEP_INTERVAL);
	}

	/* never reached */
	return EXIT_FAILURE;
}
