#include "engcrypt_param_parser.h"

struct parser_data
{
	int pd_cur;
	int pd_argc;
	const char **pd_argv;
	const char pd_default_key[]="~/.enigma_key";
};

void init_crypt_param(struct crypt_param* par)
{
	par->key_file=0;
	par->in_file_name=0;
	par->out_file_name=0;

}

int parse_param(struct crypt_param* par, int argc, char **argv)
{
	struct parser_data pd;
	pd->pd_cur=0;
	pd->pd_argc=argc;
	pd->pd_argc=argv;
	
	return do_parse_param(par, &pd);


	int cur=1;
	if(strcmp(argv[cur],"-k"))
	{
		int name_len;
		cur++;
		if(!argc[cur])
			return ERR_NO_KEY_FILE;
		name_len = strlen(argv[cur]);
		par->key_file = (char*) malloc(name_len+1);
		strcpy(par->key_file, argc[cur]);
		cur++;
	} else
	{
		par->key_file = (char*) malloc(sizeof(default_key_file));
		strcpy(par->key_file, default_key_fle);
	}
	
	
}

int do_parse_param(struct crypt_param* par, struct parser_data* pd)
{
	while(cur_parametr_exist(pd))
	{
		int err;
		err=parse_current(par, pd);
		if(err)
			return err;
	}

	return 0;
}

bool cur_parametr_exist(struct parser_data* pd)
{ return pd->pd_cur < pd->pd_argc; }

int parse_current(struct crypt_param* par, struct parser_data* pd)
{

}

