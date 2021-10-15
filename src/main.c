#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jd_profile.h"

int main(int argc, char *argv[]) {
	struct jd_profile *profile = jd_profile_new();
	jd_profile_ensure_config(profile);
	jd_profile_free(profile);
	return 0;
}
