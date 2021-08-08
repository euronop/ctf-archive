#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "log.h"
#include "state.h"

static int
state_do(sqlite3 *db, state_t **state, const char *sql) {

	sqlite3_stmt *stmt;
	int ret;

	if (!db || !sql)
		return -1;

	if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK) {
		ERROR("prepare() : %s", sqlite3_errmsg(db));
		return -1;
	}

	ret = sqlite3_step(stmt);

	if (ret == SQLITE_DONE) {
		printf("Step() No rows:\n");
		return 1;
	}

	if (ret != SQLITE_ROW) {
		ERROR("step() : %s", sqlite3_errmsg(db));
		return -1;
	}

	if (sqlite3_column_count(stmt) != 7) {
		ERROR("Wrong number of columns in state\n");
		return -1;
	}

	*state = malloc(sizeof(**state));
	if (!*state)
		return -1;
	memset(*state, 0, sizeof(**state));


	(*state)->id            = sqlite3_column_int(stmt, 0);
	(*state)->team          = sqlite3_column_int(stmt, 1);
	(*state)->service       = sqlite3_column_int(stmt, 2);
	(*state)->time_activity = sqlite3_column_int(stmt, 3);
	(*state)->sploit_active = sqlite3_column_int(stmt, 4);
	(*state)->noise_active  = sqlite3_column_int(stmt, 5);
	(*state)->handler_pid   = sqlite3_column_int(stmt, 6);

// 	const char *type        = sqlite3_column_text(stmt, 7);
// 	if (type) {
// 		(*state)->type = strdup(type);
// 	} else {
// 		(*state)->type = strdup("");
// 	}
// 
// 	const char *status      = sqlite3_column_text(stmt, 8);
// 	if (status) {
// 		(*state)->status = strdup(status);
// 	} else {
// 		(*state)->status = strdup("");
// 	}

	sqlite3_finalize(stmt);

	return 0;
}

int
state_tag(sqlite3 *db, state_t **state) {

	if (!db)
		return -1;

	char *sql = sqlite3_mprintf("UPDATE %s SET time_activity=%u, handler_pid=%d WHERE id=(SELECT id FROM %s WHERE sploit_active=1 OR noise_active=1 ORDER BY time_activity limit 1)",
					DB_BIN_STATE_TABLE, time(NULL), getpid(), DB_BIN_STATE_TABLE);

	if (sqlite3_exec(db, sql, NULL, NULL, NULL)) {
		ERROR("%s : %s", sql, sqlite3_errmsg(db));
		return -1;
	}

	if (state_get_by_pid(db, state, getpid())) {
		return -1;
	}

	return 0;
}

#if 0
int
state_unlock(sqlite3 *db, state_t *state) {

	if (!state)
		return -1;

	if (!state->handler_pid) {
		ERROR("Unlocking state with no pid");
		return -1;
	}

	char *sql = sqlite3_mprintf("UPDATE %s SET time_activity=%u, handler_pid=NULL WHERE handler_pid=%u",
			DB_BIN_STATE_TABLE, time(NULL), state->handler_pid);

	if (sqlite3_exec(db, sql, NULL, NULL, NULL)) {
		ERROR("%s : %s", sql, sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}
#endif

int
state_get_by_pid(sqlite3 *db, state_t **state, pid_t pid) {

	if (!db)
		return -1;

	char *sql = sqlite3_mprintf("SELECT id,team,service,time_activity,sploit_active,noise_active,handler_pid FROM %s WHERE handler_pid=%u",
			DB_BIN_STATE_TABLE, pid);

	if (state_do(db, state, sql))
		return -1;

	return 0;
}

int
state_get(sqlite3 *db, state_t **state, int team_id, int service_id) {

	if (!db)
		return -1;

	char *sql = sqlite3_mprintf("SELECT id,team,service,time_activity,sploit_active,noise_active,handler_pid FROM %s WHERE team=%d AND service=%d", 
			DB_BIN_STATE_TABLE, team_id, service_id);

	if (state_do(db, state, sql))
		return -1;

	return 0;
}

void
state_dump(state_t *state) {

	if (!state)
		return;

	printf("State [ id=%d, team=%d, service=%d, activity=%d, sploit_active=%d, noise_active=%d, pid=%d ]\n",
			state->id, state->team, state->service, state->time_activity, state->sploit_active, state->noise_active,
			state->handler_pid);
}
