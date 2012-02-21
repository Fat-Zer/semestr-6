#include "engcrypt.h"

struct crypt_param
{
	char *key_file;
	char *in_file_name;
	char *out_file_name;
};

int parse_param(struct crypt_param* opar, int argc, char **argv);

