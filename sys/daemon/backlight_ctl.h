#ifndef BACKLIGHT_CTL_H
#define BACKLIGHT_CTL_H

#include <stdbool.h>

#ifdef __GNUC__
#  define BLC_WARN_UNUSED  __attribute__((warn_unused_result))
#else
#  define BLC_WARN_UNUSED
#endif

enum blc_errors_t {
	BLC_OK=0,
	BLC_MALLOC_FAILED,
	BLC_READ_FAILED,
	BLC_WRIGHT_FAILED,
	BLC_OPEN_FAILED,
	BLC_CLOSE_FAILED,
	BLC_STAT_FAILED,
	BLC_OPENDIR_FAILED,
	BLC_READDIR_FAILED,
	BLC_CLOSEDIR_FAILED,
	BLC_BAD_PATH,
	BLC_NOT_DIR,
	BLC_BAD_IFACE,
	BLC_IFACE_NOT_FOUND,
	BLC_BAD_FILE_CONTENT
};

char *blc_get_iface_by_name(const char* name, int *err) BLC_WARN_UNUSED;
char *blc_get_first_iface(int *err) BLC_WARN_UNUSED;

char *blc_construct_iface_by_name(const char* name, int *err) BLC_WARN_UNUSED;

bool blc_verify_iface(const char* iface, int *err);

int blc_get_brightness(const char* iface, int *err);

void blc_set_brightness(const char* iface, int val, int *err);

int blc_get_max_brightness(const char* iface, int *err);

char *blc_strerror(int err);

#endif // BACKLIGHT_CTL_H
