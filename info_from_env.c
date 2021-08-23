#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "info_from_env.h"

#define STR_MAX_LEN 64

static int get_from_environ(pid_t pid, char *key, char* out)
{
	int sz = strlen(key);
	char filepath[STR_MAX_LEN];
	int c;
	int ret = -1;
	int k;
	int i;

	sprintf(filepath, "/proc/%d/environ", (int)pid);

	FILE *f = NULL;

	f = fopen(filepath, "r");
	if (f == NULL) {
		return -1;
	}

#define EOF_HANLER do { if (c == EOF) { \
				ret = -1; \
				goto out; \
			} \
		} while (0)

	while (1) {
		k = 0;
		while (k < sz) {
			c = fgetc(f);

			EOF_HANLER;

			if (c != key[k]) {
				break;
			}

			k++;
		}

		if (k == sz) {
			i = 0;
			while (1) {
				c = fgetc(f);
				if (c == '\0' || c == EOF) {
					break;
				}
				out[i++] = c;

				/* value too long, maybe error */
				if (i == STR_MAX_LEN - 1) {
					ret = -1;
					goto out;
				}
			}
			out[i] = '\0';
			ret = 0;
			goto out;
		}

		/* find next '\0' */
		while (1) {
			c = fgetc(f);

			EOF_HANLER;

			if (c == '\0') {
				break;
			}
		}
	}
out:
	fclose(f);
	return ret;
}

int max_cpu_count_from_env(pid_t pid)
{
	char *env_prefix;
	char env_key[STR_MAX_LEN] = {0};
	char val[STR_MAX_LEN] = {0};
	int r;

	env_prefix = getenv("INFO_FROM_ENV_PREFIX");
	if (env_prefix == NULL) {
		return -1;
	}

	sprintf(env_key, "%s%s", env_prefix, "_AVAIL_CPU_COUNT=");

	if (get_from_environ(pid, env_key, val) != 0) {
		return -1;
	}

	r = atoi(val);

	if (r > 0 && r != INT_MAX) {
		return r;
	}

	return -1;
}

uint64_t mem_limit_from_env(pid_t pid)
{
	char *env_prefix;
	char env_key[STR_MAX_LEN] = {0};
	char val[STR_MAX_LEN] = {0};
	uint64_t r;

	env_prefix = getenv("INFO_FROM_ENV_PREFIX");
	if (env_prefix == NULL) {
		return -1;
	}

	sprintf(env_key, "%s%s", env_prefix, "_AVAIL_MEM_IN_BYTES=");

	if (get_from_environ(pid, env_key, val) != 0) {
		return -1;
	}

	r = strtoull(val, NULL, 10);

	if (r > 0 && r != UINT64_MAX) {
		return r;
	}

	return -1;
}
