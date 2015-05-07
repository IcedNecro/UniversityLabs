
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "realization.c"


int main(int argc, char const *argv[])
{
	printf("%i %s\n",argc, argv[1]);
	if(argc>1){ 
		if(strcmp("mount", argv[1])==0) {
			initialize_ISO();	
		} else if(strcmp("ls", argv[1])==0) {
			ls();		
		} else if(strcmp("rm", argv[1])==0) {
			rm(argv[2]);
		} else if(strcmp("add", argv[1])==0) {
			FILE * f = fopen(argv[2], "rb+");
			put_data_to_file(f, argv[3]);
		} else if(strcmp("cp", argv[1])==0) {
			get_data_from_file(argv[2]);
		}
	} else
	return 0;	
}