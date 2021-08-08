#ifndef __SERVICE_H__
#define __SERVICE_H__

typedef struct service_t {
	int id;
	char *name;
	char *file_sploit;
	char *file_noise;
	int do_sploit;
	int do_noise;
	int sploit_freq_min;
	int sploit_freq_dif;
	int noise_freq_min;
	int noise_freq_dif;
} service_t;

int service_get(sqlite3 *db, service_t **service, int bin_id);
void service_dump(service_t *service);

#endif /* __SERVICE_H__*/
