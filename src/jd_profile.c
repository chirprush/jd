#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jd_profile.h"
#include "jd_path.h"

struct jd_profile *jd_profile_new() {
	const char *home_dir = getenv("HOME");
	if (home_dir == NULL) {
		return NULL;
	}
	struct jd_profile *profile = malloc(sizeof(struct jd_profile));
	profile->home_dir = home_dir;
	profile->current_dir = getcwd(NULL, 0);
	return profile;
}

void jd_profile_free(struct jd_profile *profile) {
	free(profile->current_dir);
	free(profile);
}

static unsigned char dir_exists(const char *path) {
	struct stat st = {0};
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

void jd_profile_ensure_config(struct jd_profile *profile) {
	struct jd_path *home_path = jd_path_from_posix(profile->home_dir);
	jd_path_append_s(home_path, ".config");
	jd_path_append_s(home_path, "jd");
	char *config_path = jd_path_to_posix(home_path);
	if (!dir_exists(config_path)) {
		mkdir(config_path, 0777);
	}
	free(config_path);
	jd_path_free(home_path);
}

struct jd_path *jd_profile_find_directory(struct jd_profile *profile, char *head) {
	if (strlen(head) == 0) {
		return jd_path_from_posix(profile->home_dir);
	}
	struct jd_path *head_path = jd_path_from_posix(head);
	if (*head == '/') {
		if (dir_exists(head)) {
			return head_path;
		}
		return NULL;
	}
	struct jd_path *joined_path = jd_path_from_posix(profile->current_dir);
	for (int i = 0; i < head_path->length; ++i) {
		jd_path_append_c(joined_path, head_path->components[i]);
	}
	struct jd_path *simplified_path;
	unsigned char relative = jd_path_is_rel(head_path);
	if (relative) {
		simplified_path = jd_path_simplify(joined_path);
		jd_path_free(joined_path);
	} else {
		simplified_path = joined_path;
	}
	unsigned char exists = jd_path_exists(simplified_path);
	jd_path_free(head_path);
	if (exists) {
		return simplified_path;
	} else if (relative) {
		jd_path_free(simplified_path);
		return NULL;
	} else {
		jd_path_free(simplified_path);
		// TODO: There is where we should put jd_profile_search_dir
		return jd_path_from_posix(profile->current_dir);
	}
}

struct jd_path *jd_profile_search_dir(struct jd_profile *profile, char *head) {
	return NULL;
}
