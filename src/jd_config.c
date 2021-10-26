#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

void jd_config_write_config(struct jd_config *config) {
	fseek(config->config_file, 0, SEEK_SET);
	freopen(NULL, "w", config->config_file);
	for (size_t i = 0; i < config->length; ++i) {
		fprintf(config->config_file, "%zu:%s\n", config->objects[i].uses, config->objects[i].path);
	}
	fclose(config->config_file);
}

unsigned char jd_config_object_path_matches(struct jd_config_object *object, const char *head);

struct jd_config_object *jd_config_find_object(struct jd_config *config, const char *head);

#undef DEFAULT_SIZE
#undef EXTEND_SIZE
