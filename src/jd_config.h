#ifndef JD_CONFIG_H
#define JD_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "jd_path.h"

struct jd_config_object {
	size_t uses;
	const char *path;
	char *source;
};

struct jd_config {
	FILE *config_file;
	struct jd_config_object *objects;
	size_t length;
	size_t capacity;
};

struct jd_config *jd_config_new();
void jd_config_free(struct jd_config *config);

void jd_config_add_object(struct jd_config *config, struct jd_config_object object);

void jd_config_sort(struct jd_config *config);
void jd_config_write_config(struct jd_config *config);

unsigned char jd_config_object_path_matches(struct jd_config_object *object, const struct jd_path *head, unsigned char is_head);
struct jd_config_object *jd_config_find_object(struct jd_config *config, const struct jd_path *head);
void jd_config_inc_use(struct jd_config *config, const struct jd_path *path);

#endif // JD_CONFIG_H
