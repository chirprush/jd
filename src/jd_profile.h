#ifndef JD_PROFILE_H
#define JD_PROFILE_H

struct jd_profile {
	const char *home_dir;
};

struct jd_profile *jd_profile_new();
void jd_profile_free(struct jd_profile *profile);

void jd_profile_ensure_config(struct jd_profile *profile);

#endif // JD_PROFILE_H
