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
	return profile;
}

void jd_profile_free(struct jd_profile *profile) {
	free(profile);
}

void jd_profile_ensure_config(struct jd_profile *profile) {
	char *dest = (char *)malloc(strlen(profile->home_dir) + sizeof("/.config/jd"));
	strcat(dest, profile->home_dir);
	strcat(dest, "/.config/jd");
	struct stat st = {0};
	if (stat(dest, &st)) {
		mkdir(dest, 0777);
	}
}
