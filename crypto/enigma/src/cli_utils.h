#ifndef CLI_UTILS_H
#define CLI_UTILS_H

#include <stddef.h>
#include <stdbool.h>
/** A bit flag type.*/
typedef size_t flag_t;

/** A enum of action type flags
 */
enum param_action_type{
	PA_NO_ACTION,
	PA_SET_BIT_FLAG, 
	PA_SET_BIT_MASK,
	PA_SET_FLAG, 
	PA_RESET_FLAG, 
	PA_GET_STRING};
/** A parameter for flag-related actions
 */
struct flag_mode{
	flag_t *flg; ///< Pointer to flag.
	flag_t mode; ///< Modificatin of flag(bits to set or mask).
};

/** struct describing action related to the parameter.
 */
struct param_action
{
	enum param_action_type type; ///< That field defines what kind of an action to do.
	/** A specific data related to the action
	 *
	 * Parametr type is depends on type field.
	 * Next variants are possible:
	 *   - for PA_NO_ACTION  - this field should be zero
	 *   - for PA_SET_FLAG   - a pointer to flag varyable
	 *   - for PA_SET_FLAG   - a pointer to flag varyable
	 *   - for PA_SET_BIT_FLAG - a pointer to flag_mode
	 *   - for PA_SET_BIT_MASK - a pointer to flag mode
	 *   - for PA_GET_STRING - char** a pointer to pointer 
	 *     to buffer
	 */
	void *data;
};

/** Descriptor of a parametr
 */
struct param_descr
{
	char s_param; ///< Short parameter view
	const char* l_param; ///< Long parameter representation
	struct param_action action; 
		///< Action applied when a parameter detected
/*	bool termenate_key;	*/
/* 		///< no short parametres alloweded after this;*/
};

/** A universal parametr parser
 *
 * @param argv - zero-terminated parameter array
 * @param pd - array of parametr_desct structs
 * @param daf_pa - 
 * @return number of parameter that were sucessfuly 
 *   paresed. Should be equal to the argc if ok.
 */
int parse_params(char *argv[],
				const struct param_descr pd[], 
				const struct param_action def_pa[]);

/** This function substitutes all vars started with $ in source string
 * and return pointer to malloced buffer.
 *
 * @param src - source string
 * @param err - out error code
 *
 * @return pointer to substituted string
 */
char* var_subst(char *dst, char* src, size_t sz);

/** counts result length of string after substitution
 *
 * @param str - string with varyablese
 * @return size of result string
 */
size_t var_subst_len(char *str);

#endif // CLI_UTILS_H
