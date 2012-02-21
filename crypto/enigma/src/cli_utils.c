#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "cli_utils.h"
/** internal parser error messages
 */
enum parser_errors {
	PE_OK, 
	PE_NOT_FOUND, 
	PE_BAD_PARAM, 
	PE_NO_OPTION,
	PE_APPLY_ACTION,
	PE_ZERO_POINTER,
	PE_NO_MORE_DEFAULT_ACTIONS,
	PE_UNKNOWN_PARAM_TYPE};

enum param_type_t {
	SHORT_KEY,
	LONG_KEY,
	OTHER};


struct param_parser_data
{
	char **argv;
	const struct param_descr *pd;
	const struct param_action *def_act;
	size_t cur;
	size_t cur_def;	///< Current deffault parametr
};



int analyse_param(struct param_parser_data *ppd);
enum param_type_t param_type(const char* str);

int analyse_long_key(struct param_parser_data *ppd);
int analyse_short_key(struct param_parser_data *ppd);
int analyse_default_param(struct param_parser_data *ppd);

const struct param_action* ident_long_key(
		const char arg[],
		const struct param_descr pd[]);
const struct param_action* ident_short_key(
		char arg,
		const struct param_descr pd[]);

int apply_action(struct param_parser_data *ppd, 
				const struct param_action* action);
int apply_set_flag(bool* flg);
int apply_reset_flag(bool* flg);
int apply_set_bit_flag(struct flag_mode* fm);
int apply_set_bit_mask(struct flag_mode* fm);
int apply_get_string(struct param_parser_data *ppd, char** dst);


int parse_params(char *argv[], const struct param_descr pd[],
				const struct param_action def_act[])
{
	struct param_parser_data ppd = {
		.argv = argv,
		.pd = pd,
		.def_act = def_act,
		.cur = 1,
		.cur_def = 0
	};
	
	while(argv[ppd.cur])
	{
		int tmp = analyse_param(&ppd);
		if(tmp != PE_OK) 
			break;
	}
	return ppd.cur;
}

int analyse_param(struct param_parser_data *ppd)
{
	switch(param_type(ppd->argv[ppd->cur]))
	{
	case LONG_KEY:
		return analyse_long_key(ppd);
	case SHORT_KEY:
		return analyse_short_key(ppd);
	case OTHER:
		return analyse_default_param(ppd);
	}
	return PE_UNKNOWN_PARAM_TYPE; // should never be here
}

enum param_type_t param_type(const char* str)
{
	if(str[0]=='-')	{
		if(str[1]=='-')	{
			if(str[2]!=0)
				return LONG_KEY;
			else
				return OTHER;
		} else if(str[1]!=0) 
				return SHORT_KEY;
			else
				return OTHER;
		
	} else
		return OTHER;
}

int analyse_long_key(struct param_parser_data *ppd)
{
	const struct param_action *action;
	action = ident_long_key(ppd->argv[ppd->cur], ppd->pd);
	if(action)
	{
		ppd->cur++;
		return apply_action(ppd,action);
	} else
		return PE_BAD_PARAM;
}

const struct param_action* ident_long_key(const char arg[], const struct param_descr pd[])
{
	size_t i;
	for(i=0; pd[i].s_param || pd[i].l_param; i++)
		if(pd[i].l_param && strcmp(arg+2,pd[i].l_param)==0)
			return &pd[i].action;
	return 0;
}

/**
 * @TODO: later rewrite for enable parametr concatenation
 */
int analyse_short_key(struct param_parser_data *ppd)
{
	const struct param_action *action;
	action = ident_short_key(ppd->argv[ppd->cur][1], ppd->pd);
	if(action)
	{
		ppd->cur++;
		return apply_action(ppd, action);
	} else
		return PE_BAD_PARAM;
}

const struct param_action* ident_short_key(
		char arg, 
		const struct param_descr pd[])
{
	size_t i;
	for(i=0; pd[i].s_param || pd[i].l_param; i++)
		if(arg==pd[i].s_param)
			return &pd[i].action;
	return 0;
}

/** @TODO expand syntax of default parametrs for optional parametrs */
int analyse_default_param(struct param_parser_data *ppd)
{
	if(!ppd->def_act[ppd->cur_def].type)
		return PE_NO_MORE_DEFAULT_ACTIONS;
	else
	{
		int rv = apply_action(ppd, &ppd->def_act[ppd->cur_def]);
		ppd->cur_def++;
		return rv;
	}
	
}

/* /////////// actions /////////////////////// */

int apply_action(
		struct param_parser_data *ppd, 
		const struct param_action* action)
{
	int rv=PE_OK;
	switch(action->type){
		case PA_NO_ACTION:
			break;
		case PA_SET_FLAG:
			rv = apply_set_flag(action->data);
			break;
		case PA_RESET_FLAG:
			rv = apply_reset_flag(action->data);
			break;
		case PA_SET_BIT_FLAG:
			rv = apply_set_bit_flag(action->data);
			break;
		case PA_SET_BIT_MASK:
			rv = apply_set_bit_mask(action->data);
			break;
		case PA_GET_STRING:
			rv = apply_get_string(ppd, action->data);
			break;
		default:
			rv = PE_APPLY_ACTION;
	}
	return rv;
}

int apply_set_bit_flag(struct flag_mode* fm)
{
	*(fm->flg) |= fm->mode;
	return PE_OK;
}

int apply_set_bit_mask(struct flag_mode* fm)
{
	*(fm->flg) &= fm->mode;
	return PE_OK;
}

int apply_set_flag(bool* flg)
{
	if(!flg)
		return PE_ZERO_POINTER;
	*flg = 1;
	return PE_OK;
}

int apply_reset_flag(bool* flg)
{
	if(!flg)
		return PE_ZERO_POINTER;
	*flg = 0;
	return PE_OK;
}

int apply_get_string(struct param_parser_data *ppd, char** dst)
{
	size_t len;
	
	if(!ppd->argv[ppd->cur])
		return PE_NO_OPTION;
	len = strlen(ppd->argv[ppd->cur]);
	if(*dst)
		free(*dst);
	*dst = malloc(len+1);
	if(!dst)
		return PE_APPLY_ACTION;
	strcpy(*dst, ppd->argv[ppd->cur]); 
	ppd->cur++;
	
	return PE_OK;
}

/* ///////////////////////////////////// */
/* /// variable-related functions    /// */
/* ///////////////////////////////////// */

size_t var_len(char* str);
char *var_find(char* str);
char* var_alloccpy_name(char* src);

size_t var_subst_len(char *str)
{
	size_t rv=0;
	char *cur, *curvar, *env, *last;
	for(cur=var_find(str); cur; last = cur, cur=var_find(cur))
	{
		size_t cur_var_len;
		rv += cur - str;
		curvar = var_alloccpy_name(cur);
		cur_var_len = var_len(cur);
		cur += cur_var_len + 1;	// count the '$' 
		env=getenv(curvar);
		if(env)
			rv += strlen(env) - cur_var_len;
		free(curvar);
	}
	rv += strlen(last);
	return rv;
}

char* var_subst(char *dst, char* src, size_t sz)
{
	char *cursrc=src, *curdst=dst;
	char *cur, *curvar, *env;
	if(!sz)
		sz=~sz;

	for(cur=var_find(cursrc); cur; cur=var_find(cursrc))
	{
		strncpy(curdst,cursrc,cur-cursrc);
		curdst+=cur-cursrc;
		curvar = var_alloccpy_name(cur);
		cursrc += var_len(cur) + 1;	// count the '$' 
		env=getenv(curvar);
		if(env) {
			strcpy(curdst,env);
			curdst+=strlen(env);
		}
		free(curvar);
	}
	strcpy(curdst,cursrc);
	
	return dst;
}

char* var_alloccpy_name(char* src)
{
	size_t len = var_len(src);
	char *rv = malloc(len+1);
	assert(rv);
	strncpy(rv, src+1, len);
	rv[len] = 0;

	return rv;
}

char *var_find(char* str)
{
	return strchr(str,'$');
}

size_t var_len(char* str)
{
	size_t i;

	for(i=1; isalnum(str[i]) || str[i]=='_'; i++);

	return i-1;
}


