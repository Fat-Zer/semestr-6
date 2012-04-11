#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "backlight_ctl.h"

#define BUF_BASE_SZ ((size_t)1 << 10)

static const char *blc_default_iface_location="/sys/class/backlight";

char *blc_pathcat(const char* dir, const char * name, int *err);
char *blc_read_fs(FILE *fs, int *err);
char *blc_read_iface_file(const char* iface, const char * name, int *err);
void blc_write_iface_file_int(const char* iface, const char * name, int val, int *err);
int blc_read_iface_file_int(const char* iface, const char * name, int *err);
FILE* blc_open_iface_file(const char* iface, const char * name, const char *mode, int *err);
char *blc_pathcat(const char* dir, const char * name, int *err);

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

char *blc_get_first_iface(int *err) {
	DIR *ls_iface=0;
	struct dirent *de=0;
	char *rv;

	ls_iface = opendir(blc_default_iface_location);
	if(!ls_iface) {
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

	while((de=readdir(ls_iface)) != 0) {
		bool found;
		rv = blc_construct_iface_by_name(de->d_name, err);
		if( rv ) {
			found = blc_verify_iface(rv, err);
			if( found ) { 
				return rv;
			}
		}

		free(rv);
	}

	*err=BLC_IFACE_NOT_FOUND;
	closedir(ls_iface);
	return 0;
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
	return blc_pathcat(blc_default_iface_location ,name, err);
}

int blc_get_brightness(const char* iface, int *err) {
	return blc_read_iface_file_int(iface, "brightness", err);
}

int blc_get_max_brightness(const char* iface, int *err) {
	return blc_read_iface_file_int(iface, "max_brightness", err);
}

void blc_set_brightness(const char* iface, int val, int *err) {
	blc_write_iface_file_int(iface, "brightness", val, err);
}

int blc_read_iface_file_int(const char* iface, const char * name, int *err) {
	char* str = blc_read_iface_file(iface, name, err);
	unsigned rv;
	int rc;
	if( *err ) {
		if( str ) {
			free(str);
		}
		return -1;
	}
	rc = sscanf(str, "%d", &rv);

	if( rc != 1 ) {
		*err = BLC_BAD_FILE_CONTENT;
		rv = 0;
	}

	free(str);
	return rv;
}

char *blc_read_iface_file(const char* iface, const char * name, int *err) {
	char *rv=0;
	
	FILE *fs = blc_open_iface_file(iface, name, "r", err);
	if(!fs) {
		return 0;
	}
	
	
	rv = blc_read_fs(fs, err);
	
	if( fclose(fs) != 0 ) {
		*err = BLC_CLOSE_FAILED;
	}

	return rv;
}

char *blc_read_fs(FILE *fs, int *err) {
	size_t sz = BUF_BASE_SZ;
	size_t readed = 0;
	char *rv = malloc( BUF_BASE_SZ );
	if( !rv ) {
		*err = BLC_MALLOC_FAILED;
		return 0;
	}
	

	do {
		size_t cur_readed = fread(rv + readed, 1, BUF_BASE_SZ, fs);
		char *tmp = 0;
		readed += cur_readed;
		if( cur_readed < BUF_BASE_SZ )
			break;
		tmp = realloc(rv, sz += BUF_BASE_SZ);

		if( !tmp ) {
			*err = BLC_MALLOC_FAILED;
			free(rv);
			return 0;
		}
	} while(1);

	if( !feof(fs) ) {
		*err = BLC_READ_FAILED;
		free(rv);
		return 0;
	}

	return rv;
}

char *blc_pathcat(const char* dir, const char * name, int *err) {
	char *rv = 0;
	size_t dir_len = strlen(dir);
	size_t name_len = strlen(name);

	rv=malloc(name_len+dir_len+2);

	if(!rv) {
		*err=BLC_MALLOC_FAILED;
		return 0;
	}

	strcpy(rv, dir);
	*(rv+dir_len) = '/';
	strcpy(rv+dir_len+1, name);
	
	return rv;
}

FILE* blc_open_iface_file(const char* iface, const char * name, const char *mode, int *err) {
	char* file= blc_pathcat(iface, name, err);
	FILE* fs;
	if( !file ) {
		return 0;
	}
	fs = fopen(file, mode);
	free(file);
	if(!fs) {
		*err = BLC_OPEN_FAILED;
	}

	return fs;
}

void blc_write_iface_file_int(const char* iface, const char * name, int val, int *err) {
	FILE *fs = blc_open_iface_file(iface, name, "w", err);
	if(!fs) {
		return;
	}
	
	fprintf(fs, "%d", val);

	if( fclose(fs) != 0 ) {
		*err = BLC_CLOSE_FAILED;
	}
}

char *blc_strerror(int err) {
	switch(err) {
	case BLC_OK:	return "BLC_OK";
	case BLC_MALLOC_FAILED:	return "BLC_MALLOC_FAILED";
	case BLC_READ_FAILED:	return "BLC_READ_FAILED";
	case BLC_WRIGHT_FAILED:	return "BLC_WRIGHT_FAILED";
	case BLC_OPEN_FAILED:	return "BLC_OPEN_FAILED";
	case BLC_CLOSE_FAILED:	return "BLC_CLOSE_FAILED";
	case BLC_STAT_FAILED:	return "BLC_STAT_FAILED";
	case BLC_OPENDIR_FAILED:                return "BLC_OPENDIR_FAILED";
	case BLC_READDIR_FAILED:	return "BLC_READDIR_FAILED";
	case BLC_CLOSEDIR_FAILED:	return "BLC_CLOSEDIR_FAILED";
	case BLC_BAD_PATH:	return "BLC_BAD_PATH";
	case BLC_NOT_DIR:	return "BLC_NOT_DIR";
	case BLC_BAD_IFACE:	return "BLC_BAD_IFACE";
	case BLC_BAD_FILE_CONTENT:	return "BLC_BAD_FILE_CONTENT";
	case BLC_IFACE_NOT_FOUND:	return "BLC_IFACE_NOT_FOUND";
	}
	return "";
}
