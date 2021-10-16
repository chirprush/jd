#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
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

static unsigned char dir_exists(const char *path) {
	struct stat st = {0};
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

void jd_profile_ensure_config(struct jd_profile *profile) {
	char *config_dir = append_strings(profile->home_dir, "/.config/jd");
	if (!dir_exists(config_dir)) {
		mkdir(config_dir, 0777);
	}
	free(config_dir);
}

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

static char *chop_dir(char *path) {
	if (strcmp(path, "/") == 0) {
		return "/";
	}
	size_t len = strlen(path);
	size_t i = len;
	while (path[i] != '/') {
		i--;
	}
	path[i] = 0;
	return path;
}

void jd_profile_find_directory(struct jd_profile *profile, char *head) {
	if (strlen(head) == 0) {
		printf("%s\n", profile->home_dir);
		return;
	}
	char *trimmed = trim_string(head);
	if (*trimmed == '/') {
		printf("%s\n", trimmed);
		return;
	} else if (strcmp(trimmed, ".") == 0) {
		printf("%s\n", profile->current_dir);
		return;
	} else if (strcmp(trimmed, "..") == 0) {
		printf("%s\n", chop_dir(profile->current_dir));
		return;
	}
	char *path_ended = append_strings(profile->current_dir, "/");
	char *path = append_strings(path_ended, trimmed);
	if (*trimmed == '.') {
		printf("%s\n", path);
	} else if (!dir_exists(path)) {
		jd_profile_search_dir(profile, trimmed);
	} else {
		printf("%s\n", path);
	}
	free(path_ended);
	free(path);
}

void jd_profile_search_dir(struct jd_profile *profile, char *head) {
}
