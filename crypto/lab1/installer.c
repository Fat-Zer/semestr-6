#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib-object.h>

#include "ssk.h"

static const char ssk[]= SSK_SIGNATURE SSK_STRUCT; 

GPtrArray* get_serial_array(GError **error);
/*
inline void check_integrity() {
	GError *error;
	int i;
	GPtrArray* serials = get_serial_array(&error);
	if( !serials ) {
		g_printf("%s\n", error->message);
		g_error_free(error);
		exit(1);
	}

	for(i=0; i<serials->len; i++) {
		if(strcmp(g_ptr_array_index(serials,i), ssk+sizeof(SSK_SIGNATURE))==0) {
			g_ptr_array_unref( serials );
			return;
		}
	}
	g_ptr_array_unref( serials );
	g_printf("The executable is broken; please reinstall the great HellowWorld program.\n");
	exit(100500);
}
*/

#define USAGE g_printf("Usage: %s prog there_to_install\n", argv[0])

int main(int argc, char *argv[]) {
	
	GMappedFile *prog_file=0;
	gchar* prog_ptr=0;
	const gchar* ssk_sig=SSK_SIGNATURE;
	GError *error=0;
	bool isFound=0;
	int i,j;
	g_type_init();
	if(argc!=3) {
		USAGE;
		exit(1);
	}
		
	program_file = g_mapped_file_new(argv[1], /* writable */ 1, &error);
	if( !prog_file ) {
		g_printf("Can not open file: %s", error->message);
		g_error_free(error);
		USAGE;
		exit(2);
	}
	prog_ptr=g_mapped_file_get_contents(program_file);
	for(i=0; i<g_mapped_file_length(program_file) && !isFound; i++)	{
		if(prog_ptr[i]==ssk_sig[i]) {
			for(j=0; j<sizeof(SSK_SIGNATURE); j++)	{
				if(prog_ptr[i+j]==ssk_sig[j] || ssk_sig[j]==0) {
					isFound=1;
				} else {
					break;
				}
			}
		}
	}
	if(!isFound) {
		g_printf("Can not find special signature in program.\n");
		USAGE;
		exit(3);
	}
	
	// select key here;
	for(j=0+size; j<; j++)	{
		prog_ptr[i+j]=
	}
	g_mapped_file_free(progrem_file);
	return 0;
}





