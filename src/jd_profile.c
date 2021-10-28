#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jd_profile.h"
#include "jd_config.h"
#include "jd_path.h"

struct jd_profile *jd_profile_new() {
	const char *home_dir = getenv("HOME");
	if (home_dir == NULL) {
		return NULL;
	}
	struct jd_profile *profile = malloc(sizeof(struct jd_profile));
	profile->home_dir = home_dir;
	profile->current_dir = getcwd(NULL, 0);
	profile->config = jd_config_new();
	return profile;
}

void jd_profile_free(struct jd_profile *profile) {
	jd_config_free(profile->config);
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

static unsigned char is_numeric(char c) {
	return c >= '0' && c <= '9';
}

static const char *chop_source_line(const char *line) {
	size_t i = 0;
	while (is_numeric(line[i])) { ++i; }
	if (i == 0) {
		return NULL;
	} else if (i >= strlen(line) || line[i] != ':') {
		return NULL;
	}
	return line + i + 1;
}

uint32_t jd_profile_read_config(struct jd_profile *profile) {
	struct jd_path *config_path = jd_path_from_posix(profile->home_dir);
	jd_path_append_s(config_path, ".config");
	jd_path_append_s(config_path, "jd");
	jd_path_append_s(config_path, "config");
	char *config_file = jd_path_to_posix(config_path);
	profile->config->config_file = fopen(config_file, "r+");
	if (profile->config->config_file == NULL) {
		profile->config->config_file = fopen(config_file, "w+");
		jd_path_free(config_path);
		free(config_file);
		return 0;
	}
	char *line = NULL;
	size_t n = 0;
	uint32_t linenr = 0;
	struct jd_config_object object = {0};
	while (getline(&line, &n, profile->config->config_file) != EOF) {
		++linenr;
		// Remove newline
		line[strlen(line) - 1] = 0;
		if (strlen(line) == 0) {
			free(line);
			line = NULL;
			continue;
		}
		object.source = line;
		sscanf(object.source, "%zu", &object.uses);
		object.path = chop_source_line(line);
		if (object.path == NULL) {
			fclose(profile->config->config_file);
			free(object.source);
			free(config_file);
			jd_path_free(config_path);
			return (linenr << 1) | 1;
		} else if (!dir_exists(object.path)) {
			free(line);
			line = NULL;
			continue;
		}
		jd_config_add_object(profile->config, object);
		line = NULL;
	}
	if (line != NULL) {
		free(line);
	}
	free(config_file);
	jd_path_free(config_path);
	return 0;
}

struct jd_path *jd_profile_find_directory(struct jd_profile *profile, char *head) {
	if (strlen(head) == 0) {
		return jd_path_from_posix(profile->home_dir);
	}
	struct jd_path *head_path = jd_path_from_posix(head);
	if (*head == '/') {
		if (dir_exists(head)) {
			jd_config_inc_use(profile->config, head_path);
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
		jd_config_inc_use(profile->config, simplified_path);
		return simplified_path;
	} else if (relative) {
		jd_path_free(simplified_path);
		return NULL;
	} else {
		jd_path_free(simplified_path);
		struct jd_path *object_path = jd_profile_lookup_dir(profile, head);
		return object_path;
	}
}

struct jd_path *jd_profile_lookup_dir(struct jd_profile *profile, char *head) {
	struct jd_path *head_path = jd_path_from_posix(head);
	struct jd_config_object *object = jd_config_find_object(profile->config, head_path);
	jd_path_free(head_path);
	if (object == NULL) {
		return NULL;
	}
	++object->uses;
	struct jd_path *object_path = jd_path_from_posix(object->path);
	return object_path;
}
