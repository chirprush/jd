#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "jd_config.h"

#define DEFAULT_SIZE 16
#define EXTEND_SIZE 16

struct jd_config *jd_config_new() {
	struct jd_config *config = malloc(sizeof(struct jd_config));
	config->config_file = NULL;
	config->objects = NULL;
	config->length = 0;
	config->capacity = 0;
	return config;
}

void jd_config_free(struct jd_config *config) {
	for (size_t i = 0; i < config->length; ++i) {
		free(config->objects[i].source);
	}
	free(config->objects);
	free(config);
}

static void ensure_objects(struct jd_config *config) {
	if (config->objects == NULL) {
		config->objects = malloc(DEFAULT_SIZE * sizeof(struct jd_config_object));
		config->length = 0;
		config->capacity = DEFAULT_SIZE;
	}
}

void jd_config_add_object(struct jd_config *config, struct jd_config_object object) {
	ensure_objects(config);
	if (config->length >= config->capacity) {
		size_t add_items = config->length - config->capacity + EXTEND_SIZE;
		size_t to_add = add_items * sizeof(struct jd_config_object);
		size_t total_size = to_add + config->capacity * sizeof(struct jd_config_object);
		config->objects = (struct jd_config_object *)realloc(config->objects, total_size);
		config->capacity += add_items;
	}
	config->objects[config->length].uses = object.uses;
	config->objects[config->length].path = object.path;
	config->objects[config->length].source = object.source;
	++config->length;
}

static int jd_config_object_compare(const void *left, const void *right) {
	size_t left_uses = ((struct jd_config_object *)left)->uses;
	size_t right_uses = ((struct jd_config_object *)right)->uses;
	if (left_uses < right_uses) {
		return 1;
	} else if (left_uses > right_uses) {
		return -1;
	}
	return 0;
}

void jd_config_sort(struct jd_config *config) {
	qsort(config->objects, config->length, sizeof(struct jd_config_object), jd_config_object_compare);
}

void jd_config_write_config(struct jd_config *config) {
	fseek(config->config_file, 0, SEEK_SET);
	config->config_file = freopen(NULL, "w", config->config_file);
	for (size_t i = 0; i < config->length; ++i) {
		fprintf(config->config_file, "%zu:%s\n", config->objects[i].uses, config->objects[i].path);
	}
	fclose(config->config_file);
}

unsigned char jd_config_object_path_matches(struct jd_config_object *object, const struct jd_path *path, unsigned char is_head) {
	struct jd_path_component *component;
	int k = strlen(object->path) - 1;
	while (k >= 0 && object->path[k] == '/') { --k; }
	for (int i = path->length - 1; i >= 0; --i) {
		component = &path->components[i];
		for (int j = component->length - 1; j >= 0; --j) {
			if (k < 0 || component->name[j] != object->path[k]) {
				return 0;
			}
			--k;
		}
		if (!is_head && i == 0 && k < 0) {
			return 1;
		} else if (is_head && i == 0 && (k >= 0 || object->path[k] == '/')) {
			return 1;
		} else if (k > 0 && object->path[k] != '/') {
			return 0;
		}
		--k;
	}
	return 1;
}

struct jd_config_object *jd_config_find_object(struct jd_config *config, const struct jd_path *head) {
	for (size_t i = 0; i < config->length; ++i) {
		if (jd_config_object_path_matches(&config->objects[i], head, 1)) {
			return &config->objects[i];
		}
	}
	return NULL;
}

void jd_config_inc_use(struct jd_config *config, const struct jd_path *path) {
	for (size_t i = 0; i < config->length; ++i) {
		if (jd_config_object_path_matches(&config->objects[i], path, 0)) {
			++config->objects[i].uses;
			return;
		}
	}
	char *posix_path = jd_path_to_posix(path);
	struct jd_config_object object = {
	    .uses = 1,
		.path = posix_path,
		.source = posix_path,
	};
	jd_config_add_object(config, object);
}

#undef DEFAULT_SIZE
#undef EXTEND_SIZE
