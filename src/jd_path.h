#ifndef JD_PATH_H
#define JD_PATH_H

#include <stdlib.h>

struct jd_path_component {
	const char *name;
	size_t length;
};

struct jd_path {
	struct jd_path_component *components;
	size_t length;
	size_t capacity;
};

struct jd_path *jd_path_new(int capacity);
void jd_path_free(struct jd_path *path);

struct jd_path *jd_path_copy(struct jd_path *path);

struct jd_path *jd_path_from_posix(const char *posix_path);
char *jd_path_to_posix(const struct jd_path *path);

unsigned char jd_path_exists(struct jd_path *path);
unsigned char jd_path_is_rel(struct jd_path *path);
void jd_path_mkdir(struct jd_path *path);

struct jd_path *jd_path_simplify(struct jd_path *path);

void jd_path_append_c(struct jd_path *path, struct jd_path_component component);
void jd_path_append_s(struct jd_path *path, const char *literal);

#endif // JD_PATH_H
