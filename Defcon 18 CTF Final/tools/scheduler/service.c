#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "db.h"
#include "service.h"

int
service_get(sqlite3 *db, service_t **service, int service_id) {

	char *sql = sqlite3_mprintf("SELECT * FROM %s WHERE id=%d", DB_SERVICE_TABLE, service_id);

	sqlite3_stmt *stmt;

	if (sqlite3_prepare(db, sql, strlen(sql), &stmt, NULL) != SQLITE_OK) {
		printf("error preparing\n");
		return -1;
	}

	if (sqlite3_step(stmt) != SQLITE_ROW) {
		printf("error stepping\n");
		return -1;
	}

	*service = malloc(sizeof(**service));
	if (!*service)
		return -1;
	memset(*service, 0, sizeof(**service));

	(*service)->id       = sqlite3_column_int(stmt, 0);
	const char *name     = sqlite3_column_text(stmt, 1);
	if (name) {
		(*service)->name = strdup(name);
	} else {
		(*service)->name = strdup("");
	}

	const char *file_sploit = sqlite3_column_text(stmt, 2);
	if (file_sploit) {
		(*service)->file_sploit = strdup(file_sploit);
	} else {
		(*service)->file_sploit = strdup("");
	}

	const char *file_noise = sqlite3_column_text(stmt, 3);
	if (file_noise) {
		(*service)->file_noise = strdup(file_noise);
	} else {
		(*service)->file_noise = strdup("");
	}

	(*service)->do_sploit       = sqlite3_column_int(stmt, 4);
	(*service)->do_noise        = sqlite3_column_int(stmt, 5);
	(*service)->sploit_freq_min = sqlite3_column_int(stmt, 6);
	(*service)->sploit_freq_dif = sqlite3_column_int(stmt, 7);
	(*service)->noise_freq_min  = sqlite3_column_int(stmt, 8);
	(*service)->noise_freq_dif  = sqlite3_column_int(stmt, 9);

	sqlite3_finalize(stmt);

	return 0;
}

void
service_dump(service_t *service) {

	printf("Service [ id=%d, name=%s, file_sploit=%s, file_noise=%s, do_sploit=%d, do_noise=%d, sploit_freq_min=%d, sploit_freq_dif=%d, noise_freq_min=%d, noise_freq_dif=%d\n",
			service->id, service->name, service->file_sploit, service->file_noise, service->do_sploit, service->do_noise,
			service->sploit_freq_min, service->sploit_freq_dif, service->noise_freq_min, service->noise_freq_dif);

}
	


