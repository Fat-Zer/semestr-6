#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib-object.h>

#define SSK_SIGNATURE "Super secrete key:"
#define SSK_STRUCT "00000000000000000000000-000000000-000000000000000000000-000000000000000000000000"

static const char ssk[]= SSK_SIGNATURE SSK_STRUCT; 

GPtrArray* get_serial_array(GError **error);

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

int main(int argc, char *argv[]) {
	g_type_init();	
	check_integrity();
	
	printf("Hello world!");

	return 0;
}


