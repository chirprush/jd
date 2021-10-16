#ifndef JD_PROFILE_H
#define JD_PROFILE_H

struct jd_profile {
	const char *home_dir;
	char *current_dir;
};

struct jd_profile *jd_profile_new();
void jd_profile_free(struct jd_profile *profile);

void jd_profile_ensure_config(struct jd_profile *profile);
const char *jd_profile_find_directory(const char *head);

#endif // JD_PROFILE_H
