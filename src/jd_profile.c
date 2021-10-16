#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include "jd_profile.h"

struct jd_profile *jd_profile_new() {
	const char *home_dir = getenv("HOME");
	if (home_dir == NULL) {
		return NULL;
	}
	struct jd_profile *profile = (struct jd_profile *)malloc(sizeof(struct jd_profile));
	profile->home_dir = home_dir;
	profile->current_dir = getcwd(NULL, 0);
	return profile;
}

void jd_profile_free(struct jd_profile *profile) {
	free(profile->current_dir);
	free(profile);
}

static char *append_strings(const char *start, const char *end) {
	char *dest = (char *)malloc(strlen(start) + strlen(end) + 1);
	*dest = 0;
	strcat(dest, start);
	strcat(dest, end);
	return dest;
}

void jd_profile_ensure_config(struct jd_profile *profile) {
	char *config_dir = append_strings(profile->home_dir, "/.config/jd");
	struct stat st = {0};
	if (stat(config_dir, &st)) {
		mkdir(config_dir, 0777);
	}
	free(config_dir);
}

const char *jd_profile_find_directory(const char *head) {
	return ".";
}
