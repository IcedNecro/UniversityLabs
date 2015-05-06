#include "declarations.h"

void initialize_ISO() {
	file_system_header * _file_system_header = malloc(sizeof(file_system_header));
	file_system * _file_system = malloc(sizeof(file_system));

	_file_system_header->_offset_file_descriptors = BIT_MAP_SIZE+sizeof(file_system_header);
	_file_system_header->_max_file_number = MAX_NUM_OF_FILES;
	_file_system_header->_block_size = BLOCK_SIZE;
	_file_system_header->_size_of_file_system = SIZE_OF_FILESYSTEM;
	_file_system_header->_max_number_of_blocks = NUMBER_OF_BLOCKS;
	_file_system_header->_data_offset = sizeof(file_system)-SIZE_OF_FILESYSTEM;
	FILE *f = fopen(PATH_TO_FS, "wb");
	
	for(int i=0; i<MAX_NUM_OF_FILES; i++)
		_file_system->file_descriptors[i]._is_free = TRUE;
	_file_system->_info = * _file_system_header;
	fwrite(_file_system,sizeof(file_system),1,f);
	
	fclose(f);	
}

file_system * get_file_system() {
	file_system * _file_system = malloc(sizeof(file_system));
	
	FILE *f = fopen(PATH_TO_FS, "rb");
	fread(_file_system, sizeof(file_system), 1, f);
	fclose(f);
	return _file_system;
}

BOOL * get_bitmap() {
	FILE *f = fopen(PATH_TO_FS, "rb");
	file_system_header * _file_system_header = get_file_system_header();

	int _offset_bitmap = sizeof(file_system_header);
	int num_of_blocks = _file_system_header->_max_number_of_blocks;

	char * bitmap = malloc(num_of_blocks);

	fseek(f, _offset_bitmap, SEEK_SET);
	fread(bitmap, num_of_blocks, 1,f);
	fclose(f);

	return bitmap;
}

file_system_header * get_file_system_header() {
	file_system_header * _file_system_header = malloc(sizeof(file_system_header));
	FILE *f = fopen(PATH_TO_FS, "rb");
	fread(_file_system_header, sizeof(file_system_header), 1, f);
	fclose(f);
	return _file_system_header;		
}

void put_data_to_file(FILE * file, char * filename) {
	char * name = filename;

	file_system_header * _file_system_header = get_file_system_header();

	printf("Current fs state:\n");
	printf("_offset_file_descriptors:%i\n",_file_system_header->_offset_file_descriptors);
	printf("_offset_data:%i\n",_file_system_header->_data_offset);

	int _offset_file_descriptors = _file_system_header->_offset_file_descriptors;
	int _num_of_files = _file_system_header->_max_file_number;
	int _offset_data = _file_system_header->_data_offset;

	fseek(file, 0L, SEEK_END);
	size_t _file_size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	
	int _blocks_required =(_file_size + BLOCK_SIZE-(_file_size%BLOCK_SIZE))/BLOCK_SIZE;

	printf("File you want to insert has size: %i and requires %i free blocks\n", _file_size, _blocks_required);
	char * file_data = malloc(_file_size);
	fread(file_data, _file_size, 1, file);

	FILE * _iso_file = fopen(PATH_TO_FS, "rb+");
	fflush(_iso_file);
	
	for(int j=0, i = _offset_file_descriptors; i<sizeof(file_descriptor)*_num_of_files; i+=sizeof(file_descriptor), j++) {
		fseek(_iso_file, i, SEEK_SET);
		file_descriptor * fd = malloc(sizeof(file_descriptor));
		fread(fd, sizeof(file_descriptor), 1, _iso_file);

		if(fd->_is_free == TRUE) {
			strcpy(fd->name, name);
			fd->index = j; 

			printf("file descriptor is free %i\n", i);
			int * _num_of_free_blocks = malloc(sizeof(int));
			*_num_of_free_blocks = 0;
			reference * _free_block_reference = seek_free_block_index(_file_system_header, _blocks_required, _iso_file, file_data);
			
			int _offset_to_seek = _offset_data + _free_block_reference->next_index * BLOCK_SIZE;

			fd->_is_free = FALSE;
			fd->reference = * _free_block_reference;			
			fd->size = _file_size;

			fseek(_iso_file, i, SEEK_SET);
			fwrite(fd, sizeof(file_descriptor), 1, _iso_file);
			free(file_data);
			free(fd);
			break;
		}
		free(fd);
	}
	free(_file_system_header);
	fclose(_iso_file);
	return;
}

reference * seek_free_block_index(file_system_header * fs_header, int required, FILE * f, const char * data) {
	int 		_bitmap_offset = sizeof(file_system_header);
	char * 		_data_ptr = data;
	int 		_data_offset = fs_header->_data_offset;
	BOOL 		is_beggining_of_file = TRUE;

	BOOL * _bitmap = get_bitmap();
	printf("recieved a bitmap\n");

	reference * ref = NULL;
	int * index_of_prev_ref = NULL;
	reference * _to_return = malloc(sizeof(reference));	
	for(int i=0; i<BIT_MAP_SIZE && required!=0; i++) {
		if(_bitmap[i] == FALSE) {
			
			int num = 0;
 			for(int j = i; j<BIT_MAP_SIZE && _bitmap[j] == FALSE && required>=0; j++, num++, required--) {
				_bitmap[j] = TRUE;
				printf("block is free %i\n", j);
			}
			printf("found %i blocks beggining from %i\n", num, i);
			
			if(is_beggining_of_file == TRUE) {
				_to_return->next_index = i;
				_to_return->num_of_blocks = num-1;
				is_beggining_of_file = FALSE;
			}

			if(ref!=NULL) {
				
				ref->next_index = i;
			}
		
			for(int j = i; j<num+i-1; j++) {
				int _offset = _data_offset + j*BLOCK_SIZE;
				printf("Writing %i bytes to block %i\n", BLOCK_SIZE, j);
				fseek(f, _offset, SEEK_SET);

				fwrite(_data_ptr, BLOCK_SIZE, 1, f);
				_data_ptr+=BLOCK_SIZE;
			} 
			printf("requires %i more blocks \n", required);
			if(ref!=NULL) {
				printf("ref isn't null\n" );

				ref->num_of_blocks = num-1;

				printf("previous ref %i - %i\n", ref->next_index, ref->num_of_blocks );

				int _offset = _data_offset + (*index_of_prev_ref) * BLOCK_SIZE;
				fseek(f, _offset,SEEK_SET);
				fwrite(ref, sizeof(reference), 1, f);
				free(ref);

			}
			if(required==-1) {
				printf("putting eof at %i\n", num+i-1);
				reference * eof = malloc(sizeof(reference));
				eof->next_index = -1;
				eof->num_of_blocks = -1;

				int _offset = _data_offset + (num+i-1) * BLOCK_SIZE;
				fseek(f, _offset, SEEK_SET);
				fwrite(eof, sizeof(reference), 1, f);
				break;
			}
			index_of_prev_ref = malloc(sizeof(int));
			*index_of_prev_ref = num+i-1;
			ref = malloc(sizeof(reference));
			required++;
		}
	}
	fseek(f, _bitmap_offset,SEEK_SET);
	fwrite(_bitmap, BIT_MAP_SIZE, 1, f);
	printf("index of free %i\n", _to_return->next_index);
	printf("num_of_blocks %i\n", _to_return->num_of_blocks);
	return _to_return;
}

file_descriptor * get_file_descriptor (char * filename) {
	file_system_header * fs_header = get_file_system_header();
	int file_descriptor_offset = fs_header->_offset_file_descriptors;
	int _max_file_number = fs_header->_max_file_number;

	FILE * _iso_file = fopen(PATH_TO_FS, "rb+");

	for(int i=0, _offset = file_descriptor_offset; i < _max_file_number; i++, _offset+=sizeof(file_descriptor)) {
		file_descriptor * fd = malloc(sizeof(file_descriptor));
		fseek(_iso_file, _offset, SEEK_SET);
		fread(fd, sizeof(file_descriptor), 1, _iso_file);
		if(fd->_is_free == FALSE && strcmp(fd->name, filename)==0) {
			printf("index of %s : %i\n", filename);
			return fd;
		}
		free(fd);
	}
}

void rm(char * filename) {

	
	file_system_header * fs_header = get_file_system_header();
	file_descriptor * fd = get_file_descriptor(filename);
	printf("You want to remove %s\n", fd->name);
	reference * _ref = &fd->reference;
	BOOL * _bitmap = get_bitmap();
	printf("Index of first blocks ; %i\n", _ref->next_index);
	printf("Num of blocks ; %i\n", _ref->num_of_blocks);

	FILE * f = fopen(PATH_TO_FS, "rb+");

	while(_ref->next_index!=-1) {
		int _next = _ref->next_index;
		int _num_of_blocks = _ref->num_of_blocks;

		for(int i = _next; i<=_next+_num_of_blocks;i++) {
			printf("(clearing %i)\n", i);
			_bitmap[i] = FALSE;
		}
		int _offset_next_reference = fs_header->_data_offset+(_next+_num_of_blocks)*BLOCK_SIZE;

		reference * _buf = malloc(sizeof(reference));

		fseek(f, _offset_next_reference, SEEK_SET);
		fread(_buf, sizeof(reference), 1, f);
//		free(_ref);
		_ref = _buf; 
	}
	fseek(f, sizeof(file_system_header), SEEK_SET);
	fwrite(_bitmap, fs_header->_max_number_of_blocks, 1, f);
	printf("(eof reached)\n");

	file_descriptor * empty_fd = malloc(sizeof(file_descriptor));
	empty_fd->_is_free = TRUE;

	int _offset_for_empty_descriptor = fs_header->_offset_file_descriptors + (fd->index)*sizeof(file_descriptor);
	printf("index of empty file %i\n, %i", fd->index, _offset_for_empty_descriptor);
	fseek(f, _offset_for_empty_descriptor, SEEK_SET);
	fwrite(empty_fd, sizeof(file_descriptor), 1, f);
	fclose(f);
	free(empty_fd);
}

void ls() {
	file_system_header * fs_header = get_file_system_header();
	int file_descriptor_offset = fs_header->_offset_file_descriptors;
	int _max_file_number = fs_header->_max_file_number;

	FILE * _iso_file = fopen(PATH_TO_FS, "rb+");

	printf("---------------Outputing list of files------------------\n" );
	for(int i=0, _offset = file_descriptor_offset; i < _max_file_number; i++, _offset+=sizeof(file_descriptor)) {
		file_descriptor * fd = malloc(sizeof(file_descriptor));
		fseek(_iso_file, _offset, SEEK_SET);
		fread(fd, sizeof(file_descriptor), 1, _iso_file);
		if(fd->_is_free == FALSE) {
			printf("%20s%20i\n", fd->name, fd->size);
		}
		free(fd);
	}
	fclose(_iso_file);
}

/*FILE * get_data_from_file(char * filename) {
	FILE * _file_to_save = fopen(filename, "wb");
	FILE * _iso_file = fopen(PATH_TO_FS, "rb+");
	file_system_header * fs_header = get_file_system_header();

	file_descriptor * fd = get_file_descriptor(filename);

	int _data_offset = fs_header->_data_offset;

	char * _file_data = malloc(fd->size);
	char * _data_ptr = _file_data;
	reference * _ref = &fd->reference;
	int j = 0;
	while(_ref->next_index!=-1) {
		int _next = _ref->next_index;
		int _num_of_blocks = _ref->num_of_blocks;

		for(int i = _next; i<=_next+_num_of_blocks-1;j++,i++) {
			fseek(_iso_file, _data_offset+(i*BLOCK_SIZE), SEEK_SET);
			fread(_data_ptr, BLOCK_SIZE, 1, _iso_file);
			_data_ptr += BLOCK_SIZE;
		}
		int _offset_next_reference = fs_header->_data_offset+(_next+_num_of_blocks)*BLOCK_SIZE;

		reference * _buf = malloc(sizeof(reference));

		fseek(_iso_file, _offset_next_reference, SEEK_SET);
		fread(_buf, sizeof(reference), 1, _iso_file);
		free(_ref);
		_ref = _buf; 	
	}
	free(_ref);
	free(fd);

	fwrite(_file_data,_file_to_save, 1, _file_to_save);
	return _file_to_save;
}*/

