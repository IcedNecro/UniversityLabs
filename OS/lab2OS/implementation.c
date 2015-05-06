
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "realization.c"


int main(int argc, char const *argv[])
{
	initialize_ISO();
	
	FILE * f = fopen("test/1","r+");
	FILE * f1 = fopen("test/2","r+");
	FILE * f2 = fopen("test/3","r+");
	FILE * f3 = fopen("test/6","rb+");
	FILE * f4 = fopen("test/7","rb+");

	put_data_to_file(f, "file_1.txt");
	put_data_to_file(f1, "file_2.txt");
	put_data_to_file(f2, "file_3.txt");
	ls();
/*	file_descriptor * d = get_file_descriptor("file_2.txt");
	printf("%s\n", d->name);*/
	rm("file_2.txt");
	
	put_data_to_file(f3, "file_4.txt");
	put_data_to_file(f3, "file_5.txt");

	ls();
	rm("file_4.txt");
	ls();
	put_data_to_file(f4, "file_6.txt");
	//rm("file_6")
	ls();
	return 0;
}