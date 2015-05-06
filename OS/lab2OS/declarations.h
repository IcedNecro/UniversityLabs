

#define BLOCK_SIZE 64					//bytes
#define MAX_NUM_OF_FILES 1024			//n
#define SIZE_OF_FILESYSTEM 262144		//bytes
#define NUMBER_OF_BLOCKS 4096			//SIZE_OF_FILESYSTEM/BLOCK_SIZE
#define BIT_MAP_SIZE 4096				//NUMBER_OF_BLOCKS/8
#define BYTES_FOR_FILENAME 20
#define NUMBER_OF_REFERENCES 15			
#define BOOL char
#define TRUE 1
#define FALSE 0
#define PATH_TO_FS "iso.iso"

typedef struct reference 
{
	int num_of_blocks;
	int next_index;
} reference;

typedef struct file_descriptor
{
	BOOL 				_is_free;
	int 				index;
	size_t 				size;
	char 				name[BYTES_FOR_FILENAME];
	reference 			reference;
} file_descriptor;

typedef struct file_system_header
{
	int 				_offset_file_descriptors;
	int 				_max_file_number;
	int 				_data_offset;
	int 				_max_number_of_blocks;
	int 				_block_size;
	int 				_size_of_file_system;	
} file_system_header;

typedef struct file_system
{
	file_system_header	_info;
	BOOL 				bit_map[BIT_MAP_SIZE];
	file_descriptor 	file_descriptors[MAX_NUM_OF_FILES];
	char 				data[SIZE_OF_FILESYSTEM];
} file_system;


void 					initialize_ISO(); 
void 					rm(char *);
void 					ls();
reference *				seek_free_block_index(file_system_header *, int, FILE *, const char *);
BOOL * 					get_bitmap();
file_descriptor * 		get_file_descriptor(char *);
file_descriptor * 		get_file_descriptors(file_system *);
FILE * 					get_data_from_file(char *);
file_system * 			get_file_system();
void 					put_data_to_file(FILE *, char *);
void 					remove_file(file_descriptor *);
file_system_header * 	get_file_system_header();

