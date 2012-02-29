#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib-object.h>
#include <stdbool.h>

#include "ssk.h"

static const char ssk[]= SSK_SIGNATURE SSK_STRUCT; 

GPtrArray* get_serial_array(GError **error);

#define USAGE g_printf("Usage: %s prog there_to_install\n", argv[0])

int main(int argc, char *argv[]) {
	
	GMappedFile *program_file=0;
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
	if( !program_file ) {
		g_printf("Can not open file: %s", error->message);
		g_error_free(error);
		USAGE;
		exit(2);
	}
	prog_ptr=g_mapped_file_get_contents(program_file);
	for(i=0; i<g_mapped_file_get_length(program_file) && !isFound; i++)	{
		if(prog_ptr[i]==ssk_sig[0]) {
			for(j=0; j<sizeof(SSK_SIGNATURE); j++)	{
				if(ssk_sig[j]==0) {
					isFound=1;
					break;
				}
				if(prog_ptr[i+j]!=ssk_sig[j]) {
					break;
				}
			}
		}
	}
	i+=sizeof(SSK_SIGNATURE)-2;
	if(!isFound) {
		g_printf("Can not find special signature in program.\n");
		USAGE;
		exit(3);
	}
	
	GPtrArray* serials = get_serial_array(&error);
	if( !serials ) {
		g_printf("%s\n", error->message);
		g_error_free(error);
		exit(4);
	}

	gchar* first_ser = g_ptr_array_index(serials,0);
		
	// select key here;
	for(j=0; first_ser[j]!=0; j++)	{
		prog_ptr[i+j]= first_ser[j];
	}
	prog_ptr[i+j]=0;
	FILE* outprog=fopen(argv[2], "w");
	if(!outprog) {
		g_printf("open file %s failed\n",argv[2]);
		exit(5);
	}
	fwrite(prog_ptr, 1, g_mapped_file_get_length(program_file), outprog);
	fclose(outprog);
	g_mapped_file_free(program_file);
	
	return 0;
}





