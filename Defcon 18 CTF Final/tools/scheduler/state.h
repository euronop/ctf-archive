#ifndef __STATE_H__
#define __STATE_H__

#include <sqlite3.h>

typedef struct state_t {
	int id;
	int team;
	int service;
	int time_activity;
	int sploit_active;
	int noise_active;
	pid_t handler_pid;
// 	char *type;
// 	char *status;
} state_t;


int state_get_by_pid(sqlite3 *db, state_t **state, pid_t pid);
int state_get(sqlite3 *db, state_t **state, int team_id, int service_id);
int state_tag(sqlite3 *db, state_t **state);
void state_dump(state_t *state);

#endif /* __STATE_H__ */
