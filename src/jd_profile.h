#ifndef JD_PROFILE_H
#define JD_PROFILE_H

#include <stdint.h>

#include "jd_config.h"

struct jd_profile {
	const char *home_dir;
	char *current_dir;
	struct jd_config *config;
};

struct jd_profile *jd_profile_new();
void jd_profile_free(struct jd_profile *profile);

void jd_profile_ensure_config(struct jd_profile *profile);
uint32_t jd_profile_read_config(struct jd_profile *profile);

struct jd_path *jd_profile_find_directory(struct jd_profile *profile, char *head);
struct jd_path *jd_profile_search_dir(struct jd_profile *profile, char *head);

#endif // JD_PROFILE_H
