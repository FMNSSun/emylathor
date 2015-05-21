#include "common/definitions.h"
#include "lib_vasmdis.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <pthread.h>
#include <gio/gio.h>
#include "vdis.h"

char* vdis_load_img(char* path, gsize* length) {
	GFile* infile = g_file_new_for_path(path);

	char* filedata = NULL;
	
	gboolean suc = g_file_load_contents(
		infile,
		NULL,
		&filedata,
		length,
		NULL,
		NULL);
	

	if(!suc)
	{
		fprintf(stderr, "Could not read image file. Sorry.\n");
		exit(11);
	}

	return filedata;
}

int main(int argc, char* argv[]) {
	#if !defined(GLIB_VERSION_2_36)
	  g_type_init();
	#endif

	char* img_name;
	char* output_name;
	  
	if(argc < 3){
		usage(argv[0]);
		exit(1);
	}else if(!strcmp(argv[1],"--img")){
		img_name = argv[2];
		
		if(argc > 4 && !strcmp(argv[3],"--outfile")){
			output_name = argv[4];
		}else{
			output_name = "stdout";
		}
	}else{
		printf("unknown option %s", argv[1]);
		exit(1);
	}
	  
	gsize size = 0;
	char* filedata = vdis_load_img(img_name, &size);
	uint32_t pos = 0;
	
	FILE * file;
	char line [100];

	if(!strcmp(output_name, "stdout")){
		file = stdout;
		
	}else{
		file = fopen (output_name , "w");
	}
	
	if (file == NULL) {
		perror ("Error opening file");
		exit(-1);
	}
	
	while(pos < size) {
		fprintf(file, "%s\n",vasmdis_disasm_instruction(&filedata[pos]));
		pos += 4;
	}
	
	fclose(file);
}

void usage(char* name) {
	printf("Usage\n");
	printf(" %s --img <input file> [--outfile <output_file>] \n\n", name);
}
