#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "db.h"
#include "log.h"
#include "bin_worker.h"

int
bin_worker_new(sqlite3 *db, state_t *state, char *type, int *id) {

	if (!db || !state)
		return -1;

	char *sql = sqlite3_mprintf("INSERT INTO %s (team, service, time_started, pid, type, status) VALUES (%d,%d,%d,%d,'%q', '%q')",
		 DB_BIN_WORKER_TABLE, state->team, state->service, time(NULL), getpid(), type?type:"", "started");

	if (sqlite3_exec(db, sql, NULL, NULL, NULL)) {
		ERROR("%s : %s", sql, sqlite3_errmsg(db));
		return -1;
	}

	*id = sqlite3_last_insert_rowid(db);
	return 0;
}

int
bin_worker_get_last(sqlite3 *db, state_t *state, int *time_started, int *time_ended, char *type) {

	struct {
		char *time_started;
		char *time_ended;
	} *rows;
	int nrows;
	int ncols;
	char *errmsg;

	char *sql = sqlite3_mprintf("SELECT time_started,time_ended FROM %s WHERE team=%d AND service=%d AND type='%q' AND status='ended' ORDER BY time_started DESC LIMIT 1",
			DB_BIN_WORKER_TABLE, state->team, state->service, type);

	if (sqlite3_get_table(db, sql, (char ***)&rows, &nrows, &ncols, &errmsg) != SQLITE_OK) {
		FATAL("sqlite3_get_table() failed: %s", errmsg);
	}


	if (nrows == 0) {
		/* no rows */
		*time_started = 0;
		*time_ended = 0;

		return 0;
	}

	if (nrows != 1) {
		FATAL("nrows != 1\n");
	}

	if (ncols != 2) {
		FATAL("ncols != 2\n");
	}

	if (rows[1].time_started) {
		*time_started = atoi(rows[1].time_started);
	} else {
		*time_started = 0;
	}
	if (rows[1].time_ended) {
		*time_ended = atoi(rows[1].time_ended);
	} else {
		*time_ended = 0;
	}

	return 0;
}

int
bin_worker_done(sqlite3 *db, int id) {

	char *sql = sqlite3_mprintf("UPDATE %s SET time_ended=%d,status='ended' WHERE id=%d", DB_BIN_WORKER_TABLE, time(NULL), id);

	if (sqlite3_exec(db, sql, NULL, NULL, NULL)) {
		ERROR("%s : %s", sql, sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}
