#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include "jd_path.h"

#define DEFAULT_SIZE 16
#define EXTEND_SIZE 16

struct jd_path *jd_path_new(int capacity) {
	struct jd_path *path = malloc(sizeof(struct jd_path));
	if (capacity != 0) {
		path->components = malloc(capacity * sizeof(struct jd_path_component));
	} else {
		path->components = NULL;
	}
	path->length = 0;
	path->capacity = capacity;
	return path;
}

void jd_path_free(struct jd_path *path) {
	if (path->components) { 
		free(path->components);
	}
	free(path);
}

struct jd_path *jd_path_copy(struct jd_path *path) {
	struct jd_path *copy = malloc(sizeof(struct jd_path));
	copy->components = malloc(path->capacity * sizeof(struct jd_path_component));
	memcpy(copy->components, path->components, path->capacity * sizeof(struct jd_path_component));
	copy->capacity = path->capacity;
	copy->length = path->length;
	return copy;
}

struct jd_path *jd_path_from_posix(const char *posix_path) {
	struct jd_path *path = jd_path_new(0);
	unsigned char skip = 0;
	size_t len = strlen(posix_path);
	struct jd_path_component current = {0};
	for (int i = 0; i < len; ++i) {
		if (posix_path[i] == '/') {
			if (current.name) {
				current.length = (posix_path + i) - current.name;
				jd_path_append_c(path, current);
			}
			skip = 0;
			continue;
		} else if (i == len - 1) {
			if (current.name == NULL) {
				current.name = posix_path;
			}
			current.length = (posix_path + i) - current.name + 1;
			jd_path_append_c(path, current);
		} else if (skip) {
			continue;
		}
		current.name = posix_path + i;
		skip = 1;
	}
	return path;
}

char *jd_path_to_posix(const struct jd_path *path) {
	if (path->length == 0) {
		char *root_path = malloc(2);
		root_path[0] = '/';
		root_path[1] = 0;
		return root_path;
	}
	size_t length = 0;
	for (int i = 0; i < path->length; ++i) {
		length += path->components[i].length + 1;
	}
	char *posix_path = malloc(length + 1);
	posix_path[length] = 0;
	char *iter = posix_path;
	for (int i = 0; i < path->length; ++i) {
		*iter = '/';
		++iter;
		for (int j = 0; j < path->components[i].length; ++j) {
			*iter = path->components[i].name[j];
			++iter;
		}
	}
	return posix_path;
}

unsigned char jd_path_exists(struct jd_path *path) {
	struct stat st = {0};
	char *posix_path = jd_path_to_posix(path);
	unsigned char exists = stat(posix_path, &st) == 0 && S_ISDIR(st.st_mode);
	free(posix_path);
	return exists;
}

static unsigned char component_relative(const struct jd_path_component *c) {
	return (c->length == 2 && c->name[0] == '.' && c->name[1] == '.') || (c->length == 1 && c->name[0] == '.');
}

unsigned char jd_path_is_rel(struct jd_path *path) {
	if (path->length == 0) {
		return 0;
	}
	for (int i = 0; i < path->length; ++i) {
		if (component_relative(&path->components[i])) {
			return 1;
		}
	}
	return 0;
}

void jd_path_mkdir(struct jd_path *path) {
	char *posix_path = jd_path_to_posix(path);
	mkdir(posix_path, 0777);
	free(posix_path);
}

struct jd_path *jd_path_simplify(struct jd_path *path) {
	struct jd_path *new_path = jd_path_new(path->length);
	for (int i = 0; i < path->length; ++i) {
		if (component_relative(&path->components[i])) {
			if (path->components[i].length == 1 && *path->components[i].name == '.') {
				continue;
			} else if (new_path->length != 0) {
				new_path->length -= 1;
			}
		} else {
			jd_path_append_c(new_path, path->components[i]);
		}
	}
	return new_path;
}

static void ensure_components(struct jd_path *path) {
	if (path->components == NULL) {
		path->components = malloc(DEFAULT_SIZE * sizeof(struct jd_path_component));
		path->length = 0;
		path->capacity = DEFAULT_SIZE;
	}
}

void jd_path_append_c(struct jd_path *path, struct jd_path_component component) {
	ensure_components(path);
	if (path->length >= path->capacity) {
		size_t add_items = path->length - path->capacity + EXTEND_SIZE;
		size_t to_add = add_items * sizeof(struct jd_path_component);
		size_t total_size = to_add + path->capacity * sizeof(struct jd_path_component);
		path->components = (struct jd_path_component *)realloc(path->components, total_size);
		path->capacity += add_items;
	}
	path->components[path->length].name = component.name;
	path->components[path->length].length = component.length;
	++path->length;
}

void jd_path_append_s(struct jd_path *path, const char *literal) {
	struct jd_path_component component = { literal, strlen(literal) };
	jd_path_append_c(path, component);
}

#undef DEFAULT_SIZE
#undef EXTEND_SIZE
