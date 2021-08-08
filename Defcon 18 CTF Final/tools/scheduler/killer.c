#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "killer.h"

#include "db.h"
#include "log.h"

sqlite3 *db;

extern char *db_file;

int 
killer(void) {

	struct {
		char *id;
		char *time_started;
		char *pid;
	} *rows;
	int nrows;
	int ncols;
	int i,j;
	char *sql = NULL;

	if (sqlite3_open(db_file, &db) != SQLITE_OK) {
		FATAL("sqlite3_open() failed");
	}
	sqlite3_busy_timeout(db, 60000);

	if (sqlite3_get_table(db, "SELECT id,time_started, pid FROM bin_worker WHERE status='started'", (char ***)&rows, &nrows, &ncols, NULL) != SQLITE_OK) {
		FATAL("sqlite3_get_table() failed: %s", sqlite3_errmsg(db));
	}

	if (nrows == 0) {
		return 0;
	}

	if (ncols != 3) {
		FATAL("ncols is %d, not 3", ncols);
	}

	time_t now = time(NULL);

	for (i=1,j=0; i<=nrows; i++) {

		if (!rows[i].pid || !rows[i].id || !rows[i].time_started)
			continue;

		/* kill anything taking more than 3 minutes to complete */
		if ( (now - atoi(rows[i].time_started) > 180) ) {

			/* kill */
			int ret = kill(atoi(rows[i].pid), 0);
			if (ret) {
				sql = sqlite3_mprintf("UPDATE %s SET status='cleaned' WHERE id='%q'", DB_BIN_WORKER_TABLE, rows[i].id);
			} else {
				sql = sqlite3_mprintf("UPDATE %s SET status='killed' WHERE id='%q'", DB_BIN_WORKER_TABLE, rows[i].id);
			}

			if (sqlite3_exec(db, sql, NULL, NULL, NULL)) {
				FATAL("sqlite3_exec() failed: %s", sqlite3_errmsg(db));
			}

			printf("purged: id=%s, time=%s, pid=%s\n", rows[i].id, rows[i].time_started, rows[i].pid);

			j++;

			sqlite3_free(sql);
		}

	}

// 	printf("Killed/Cleaned %d entries\n", j);

	sqlite3_free_table((char **)rows);

	sqlite3_close(db);

	return 0;
}
