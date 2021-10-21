#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jd_profile.h"
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
	if (argc == 1) {
		printf("%s\n", profile->home_dir);
	} else {
		char *arg = trim_string(argv[1]);
		struct jd_path *path = jd_profile_find_directory(profile, arg);
		if (path == NULL) {
			fprintf(stderr, "jd: could not resolve path '%s'\n", arg);
			jd_profile_free(profile);
			return EXIT_FAILURE;
		}
		char *posix_path = jd_path_to_posix(path);
		printf("%s\n", posix_path);
		free(posix_path);
		jd_path_free(path);
	}
	jd_profile_free(profile);
	return 0;
}
