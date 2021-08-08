#ifndef __DB_H__
#define __DB_H__

#define DB_FILE "ctf.db"

#define DB_TEAM_TABLE            "team"
#define DB_TEAM_CREATE           "CREATE TABLE IF NOT EXISTS "DB_TEAM_TABLE \
                                 "(id INTEGER PRIMARY KEY, ip TEXT, name TEXT)"

#define DB_SERVICE_TABLE         "service"
#define DB_SERVICE_CREATE        "CREATE TABLE IF NOT EXISTS "DB_SERVICE_TABLE \
                                 "(id INTEGER PRIMARY KEY, name TEXT, file_sploit TEXT, file_noise TEXT, do_sploit BOOLEAN, do_noise BOOLEAN, "\
                                 "freq_sploit_min INTEGER, freq_sploit_dif INTEGER, freq_noise_min INTEGER, freq_noise_dif INTEGER) "


#define DB_BIN_WORKER_TABLE      "bin_worker"
#define DB_BIN_WORKER_CREATE     "CREATE TABLE IF NOT EXISTS "DB_BIN_WORKER_TABLE \
                                 "(id INTEGER PRIMARY KEY, service INTEGER, team INTEGER, time_started INTEGER, time_ended INTEGER, pid INTEGER, type TEXT, status TEXT,"\
                                 "done INTEGER ,"\
                                 "FOREIGN KEY(team) REFERENCES "DB_TEAM_TABLE"(id), FOREIGN KEY(service) REFERENCES "DB_SERVICE_TABLE"(id) )" 

#define DB_BIN_STATE_TABLE       "bin_state"
#define DB_BIN_STATE_CREATE      "CREATE TABLE IF NOT EXISTS "DB_BIN_STATE_TABLE \
                                 "(id INTEGER PRIMARY KEY, team INTEGER, service INTEGER, time_activity INTEGER, "\
                                 "sploit_active BOOLEAN, noise_active BOOLEAN, last_status TEXT, handler_pid INTEGER, "\
                                 "FOREIGN KEY(team) REFERENCES "DB_TEAM_TABLE"(id), FOREIGN KEY(service) REFERENCES "DB_SERVICE_TABLE"(id) )"

extern char *db_file;

#endif /* __DB_H__ */
