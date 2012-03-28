#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "backlight_ctl.h"

static const char *blc_default_iface_location="/sys/class/backlight";


char *blc_get_iface_by_name(const char* name, int *err) {
	char *rv = blc_construct_iface_by_name(name, err);

	if(!rv) {
		return 0;
	}
	if(!blc_verify_iface(rv, err)) {
		free(rv);
		return 0;
	} else {
		return rv;
	}
}

bool blc_verify_iface(const char* iface, int *err) {
	DIR *ifaced=0;
	struct dirent *de=0;
	bool has_brightness=0, has_max_brightness=0;

	ifaced = opendir(iface);
    if(!ifaced) {
		switch(errno) {
		case ENOTDIR:
			*err = BLC_NOT_DIR;
			errno = 0;
			break;
		case ENOENT:
			*err = BLC_BAD_PATH;
			errno = 0;
			break;
		default:
			*err = BLC_OPENDIR_FAILED;
		}
		return 0;
	}

	while( (de=readdir(ifaced)) != 0 ) {
		if(strcmp(de->d_name, "brightness")==0) {
			has_brightness=1, has_max_brightness;
		} else if(strcmp(de->d_name, "max_brightness")==0) {
			has_max_brightness=1, has_max_brightness;
		}
	}

	if(errno) {
		*err=BLC_READDIR_FAILED;
		closedir(ifaced);
		return 0;
	}

	if(!has_brightness || !has_max_brightness) {
		*err=BLC_BAD_IFACE;
		closedir(ifaced);
		return 0;
	}

	if(closedir(ifaced)==-1) {
		*err=BLC_CLOSEDIR_FAILED;
		return 0;
	}

	return 1;
}

char *blc_construct_iface_by_name(const char* name, int *err) {
	char *rv = 0;
	size_t def_loc_len = strlen(blc_default_iface_location);
	size_t name_len = strlen(name);

	rv=malloc(name_len+def_loc_len+2);

	if(!rv) {
		*err=BLC_MALLOC_FAILED;
		return 0;
	}

	strcpy(rv, blc_default_iface_location);
	*(rv+def_loc_len) = '/';
	strcpy(rv+def_loc_len+1, name);
	
	return rv;
}
