#include <sqlite3.h>

#include "state.h"

typedef struct bin_worker_t {
	int id;
	int team;
	int service;
	int time_started;
	int time_ended;
	int pid;
	char *type;     // "sploit" or "noise"
	char *status;
	int backdoored; // move to state table
	char *token_harvested;
} bin_worker_t;

typedef struct harvest_t {
	int id;
	int time_harvested;
	int time_submitted;
} harvest_t;

// self sploitating as uptime test

// int bin_worker_get(sqlite3 *db, bin_worker_t **bin_worker, char *type, int team, int service);
int bin_worker_new(sqlite3 *db, state_t *state, char *type, int *id);
int bin_worker_get_last(sqlite3 *db, state_t *state, int *time_started, int *time_ended, char *type);
