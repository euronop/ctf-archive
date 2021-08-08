#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include "db.h"
#include "log.h"
#include "service.h"
#include "state.h"
#include "bin_worker.h"



sqlite3 *db;

void
log_file(char *str) {

	FILE *fp = fopen("log.txt", "a+");

	fwrite(str, strlen(str)+1, 1, fp);

	fclose(fp);
}


static int
do_spawn(sqlite3 *db, service_t *service, int team_id, int type) {

	struct  {
		char *ip;
		char *name;
	} *rows;
	int nrows;
	int ncols;
	char *errmsg;

	char *sql = sqlite3_mprintf("SELECT ip,name FROM %s WHERE id=%u", DB_TEAM_TABLE, team_id);

	if (sqlite3_get_table(db, sql, (char ***)&rows, &nrows, &ncols, &errmsg) != SQLITE_OK) {
		FATAL("Error getting team: %s", errmsg);
	}

	if (nrows == 0) {
		FATAL("No such team %u\n", team_id);
	}

	if (ncols != 2) {
		FATAL("Wrong number of columns");
	}

	pid_t pid;
	char name[512];
	char *argv[] = { name, rows[1].ip, NULL };
	char a[1024];

	pid = fork();

	if (pid < 0) {
		perror("killer fork() failed");
		return;

	} else if (pid == 0) {
		if (type == 1) {
			snprintf(name, sizeof(name)-1, "%s", service->file_sploit);
		} else {
			snprintf(name, sizeof(name)-1, "%s", service->file_noise);
		}

		printf("Spawning [%d]: '%s %s'\n", getpid(), name, rows[1].ip);

		if (type == 1) {
			snprintf(a, 1024, "%s %s | ./insert_token.pl", name,  rows[1].ip);
			printf("%s\n", a);
			system(a);
                } else {
                        execve(argv[0], argv, NULL);
                }

		_exit(EXIT_FAILURE);
	}

	/* parent */
	wait(NULL);

	return 0;
}


int
spawn_sploit(sqlite3 *db, service_t *service, int team_id) {

	return do_spawn(db, service, team_id, 1);
}

int
spawn_noise(sqlite3 *db, service_t *service, int team_id) {

	return do_spawn(db, service, team_id, 0);
}



int
sploit_worker(void) {

	struct sploit_worker_args2 *rows;
	char query[1024];
	int nrows;
	int ncols;
	char *err_msg;
	int delay;
	service_t *service;
	state_t *state;

	//openlog("sploit_worker", LOG_PID, LOG_USER);

	/* connect to db */
	if (sqlite3_open(db_file, &db) != SQLITE_OK) {
		FATAL("sqlite3_open() failed");
	}

	sqlite3_busy_timeout(db, 60000);

	/* get the oldest state and tag it with our pid */
	if (state_tag(db, &state)) {
		FATAL("Unabled to get oldest state");
	}

	//state_dump(state);

	/* get service */
	if (service_get(db, &service, state->service)) {
		FATAL("Error getting service with id %d", state->service);
	}

	//service_dump(service);

	srand(time(NULL) ^ getpid());

	/* launch either noise or sploit */
	//printf("state active = %d, service active = %d\n", state->sploit_active, service->do_sploit);
	if (state->sploit_active && service->do_sploit) {

		time_t now = time(NULL);
		int time_start;
		int time_end;

		/* get the latest launched exploit */
		if (bin_worker_get_last(db, state, &time_start, &time_end, "sploit")) {
			ERROR("bin_worker_get_last_sploit() failed\n");
			goto noise;
		}

		/* latest exploit is still running */
		if (time_start && (time_end == 0)) {
			//printf("latest exploit is still running\n");
			goto noise;
		}

		/* check if its too soon to exploit */
		if ( now - time_start < service->sploit_freq_min ) { // this is PER TEAM. One team will completely take over being the only one getting run
			//printf("too soon to sploit, goto noise\n");
			goto noise;
		}

		/* let's go */
		//printf("Exploiting\n");

		/* add new bin_worker */
		int new_worker_id;
		if (bin_worker_new(db, state, "sploit", &new_worker_id)) {
			FATAL("Failed adding new bin_worker entry\n");
		}

		/* delay the sploit a bit */
		delay = (state->time_activity + service->noise_freq_min + service->noise_freq_dif) - now;
		if (delay > 0) {
			delay = (int)((float)delay * rand() / RAND_MAX);
			sleep(delay);
		}
		//printf("sploit delay = %d\n", delay);


		if (spawn_sploit(db, service, state->team)) {
			FATAL("Child failed exploting\n");
		}

		/* we're done, update bin_worker */
		if (bin_worker_done(db, new_worker_id)) {
			FATAL("Failed finishing bin worker entry\n");
		}

		goto end;
	}

noise:
	/* do noise */

	if (state->noise_active && service->do_noise) {

		//printf("Noise\n");

		time_t now  = time(NULL);
		int time_start;
		int time_end;

		/* get the latest launched noise */
		if (bin_worker_get_last(db, state, &time_start, &time_end, "noise")) {
			ERROR("bin_worker_get_last_sploit() failed\n");
			goto end;
		}

		//printf(" [ last run start = %d, end = %d\n", time_start, time_end);

		/* check if its too soon to noise again */
		if ( now - time_start < service->noise_freq_min ) {
// 			printf("skipping noise, now=%u, time_activity=%d\n", (int)now, state->time_activity);
			goto end;
		}

		/* add new bin_worker */
		int new_worker_id;
		if (bin_worker_new(db, state, "noise", &new_worker_id)) {
			FATAL("Failed adding new bin_worker entry\n");
		}

		/* delay noise a bit */
		delay = (state->time_activity + service->noise_freq_min + service->noise_freq_dif) - now;
		if (delay > 0) {
			delay = (int)((float)delay * rand() / RAND_MAX);
			//printf("noise delay = %d\n", delay);
			sleep(delay);
		}

		if (spawn_noise(db, service, state->team)) {
			FATAL("Child failed noising\n");
		}

		/* we're done, update bin_worker */
		if (bin_worker_done(db, new_worker_id)) {
			FATAL("Failed finishing bin worker entry\n");
		}
	}

end:
	return 0;

}


