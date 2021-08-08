/* initialize db with sane stuff */

#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "db.h"
#include "log.h"

char *db_file = DB_FILE;

static int
init_db(char *db_file) {

	sqlite3 *db = NULL;
	unsigned int i,j;

	/* connect to db */
	if (sqlite3_open(db_file, &db) != SQLITE_OK) {
		FATAL("sqlite3_open() failed");
	}

	/* create team table */
	if(sqlite3_exec(db, DB_TEAM_CREATE, NULL, NULL, NULL)) {
		FATAL("%s : %s", DB_TEAM_CREATE, sqlite3_errmsg(db));
	}

	/* create service table */
	if(sqlite3_exec(db, DB_SERVICE_CREATE, NULL, NULL, NULL)) {
		FATAL("%s : %s", DB_SERVICE_CREATE, sqlite3_errmsg(db));
	}

	/* create bin worker table */
	if(sqlite3_exec(db, DB_BIN_WORKER_CREATE, NULL, NULL, NULL)) {
		FATAL("%s : %s", DB_BIN_WORKER_CREATE, sqlite3_errmsg(db));
	}

	/* create sploit state table */
	if(sqlite3_exec(db, DB_BIN_STATE_CREATE, NULL, NULL, NULL)) {
		FATAL("%s : %s", DB_BIN_STATE_CREATE, sqlite3_errmsg(db));
	}

	/* bootstrap team table */
	for(i=1; i<=10; i++) {

		char query[512];
		char ip[64];
		char team[64];

		snprintf(team, sizeof(team)-1, "team%02u", i);
		snprintf(ip, sizeof(ip)-1, "10.31.%u.1", i);

		sqlite3_snprintf(sizeof(query)-1, query, "INSERT OR IGNORE INTO %s (id, ip, name) VALUES (%u, '%q', '%q')", DB_TEAM_TABLE, i, ip, team);

		if (sqlite3_exec(db, query, NULL, NULL, NULL)) {
			FATAL("%s : %s", query, sqlite3_errmsg(db));
		}
	}

	/* bootstrap services */
	for(i=1; i<=14; i++) {

		char query[512];
		char name[64];
		char file_sploit[64];
		char file_noise[64];

		snprintf(name, sizeof(name)-1, "service%02u", i);
		snprintf(file_sploit, sizeof(file_sploit)-1, "./sploits/%d-rigtig", i);
		snprintf(file_noise, sizeof(file_noise)-1, "./sploits/%d-noise", i);

		sqlite3_snprintf(sizeof(query)-1, query, "INSERT OR IGNORE INTO %s (name,file_sploit,file_noise,do_sploit,do_noise,freq_sploit_min,freq_sploit_dif,freq_noise_min,freq_noise_dif) VALUES ('%q','%q','%q',0,0,300,5,60,5)", DB_SERVICE_TABLE, name, file_sploit, file_noise);

		if (sqlite3_exec(db, query, NULL, NULL, NULL)) {
			FATAL("%s : %s", query, sqlite3_errmsg(db));
		}


	}


	/* bootstrap state table */
	for(i=1; i<=10; i++) { // for each team
		for(j=1; j<=14; j++) { // for each service

			char query[512];
			time_t t1 = time(NULL) - rand() % 3600;

			sqlite3_snprintf(sizeof(query)-1, query, "INSERT OR IGNORE INTO %s (team, service, time_activity, sploit_active, noise_active) "\
                                                     "VALUES (%u, %u, %d, %u, %u)", DB_BIN_STATE_TABLE, i, j, t1, 0, 0);

			if (sqlite3_exec(db, query, NULL, NULL, NULL)) {
				FATAL("%s : %s", query, sqlite3_errmsg(db));
			}

		}
	}


	sqlite3_close(db);

	return 0;
}
int
main(int argc, char **argv) {

	if (argc > 3) {
		printf("usage: %s [db-file]\n", argv[0]);
	}

	if (argc == 2) {
		db_file = argv[1];
	}

	if (init_db(db_file)) {
		FATAL("Error initializing database");
	}

	return 0;
}
