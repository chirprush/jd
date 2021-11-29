#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jd_profile.h"
#include "jd_config.h"
#include "jd_path.h"

static char *trim_string(char *string) {
	char *result = string;
	while (*result == ' ') result++;
	size_t len = strlen(result);
	size_t i = len - 1;
	while (result[i] == ' ') {
		i--;
	}
	i++;
	result[i] = 0;
	return result;
}

int main(int argc, char *argv[]) {
	struct jd_profile *profile = jd_profile_new();
	jd_profile_ensure_config(profile);
	uint32_t config_status = jd_profile_read_config(profile);
	int exit_status = 0;
	if (config_status & 1) {
		fprintf(stderr, "jd: line %d: error while reading config file\n", config_status >> 1);
		exit_status = EXIT_FAILURE;
		goto free_profile;
	}
	if (argc == 1) {
		printf("%s\n", profile->home_dir);
	} else if (strcmp(argv[1], "-l") == 0) {
		for (size_t i = 0; i < profile->config->length; ++i) {
			struct jd_config_object object = profile->config->objects[i];
			struct jd_path *path = jd_path_from_posix(object.path);
			printf("%s\n", path->components[path->length-1].name);
			jd_path_free(path);
		}
	} else {
		char *arg = trim_string(argv[1]);
		struct jd_path *path = jd_profile_find_directory(profile, arg);
		if (path == NULL) {
			fprintf(stderr, "jd: could not resolve path '%s'\n", arg);
			exit_status = EXIT_FAILURE;
			goto free_all;
		}
		char *posix_path = jd_path_to_posix(path);
		printf("%s\n", posix_path);
		free(posix_path);
		jd_path_free(path);
	}
	jd_config_sort(profile->config);
 free_all:
	jd_config_write_config(profile->config);
 free_profile:
	jd_profile_free(profile);
	return exit_status;
}
