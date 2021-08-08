#ifndef __LOG_H__
#define __LOG_H__

#define ERROR(s, ...) \
	do { \
		printf("%s: " s "\n", __func__, ##__VA_ARGS__); \
	} while (0)

#define FATAL(s, ...) \
	do { \
		printf("%s: " s "\n", __func__, ##__VA_ARGS__); \
		return -1; \
	} while (0)

#endif /* __LOG_G__ */
