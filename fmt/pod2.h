#ifndef __POD2__
#define __POD2__
#include <sys/types.h>
#include <sys/stat.h>

#include "pod_common.h"

#define POD_HEADER_POD2_SIZE (POD_HEADER_IDENT_SIZE + \
                              POD_HEADER_CHECKSUM_SIZE + \
			      POD_HEADER_COMMENT_SIZE + \
			      POD_HEADER_FILE_COUNT_SIZE + \
			      POD_HEADER_AUDIT_FILE_COUNT_SIZE)


/* 20 bytes */
#define POD_DIR_ENTRY_POD2_SIZE (POD_DIR_ENTRY_PATH_OFFSET_SIZE + \
                                 POD_DIR_ENTRY_SIZE_SIZE + \
				 POD_DIR_ENTRY_OFFSET_SIZE + \
				 POD_DIR_ENTRY_TIMESTAMP_SIZE + \
				 POD_DIR_ENTRY_CHECKSUM_SIZE + \
				 POD_DIR_ENTRY_FILENAME_SIZE)

#define POD_DIR_ENTRY_POD2_FILENAME_SIZE     POD_DIR_ENTRY_FILENAME_SIZE

#define POD_AUDIT_ENTRY_POD2_SIZE (POD_AUDIT_ENTRY_USER_SIZE + \
				   POD_AUDIT_ENTRY_TIMESTAMP_SIZE + \
				   POD_AUDIT_ENTRY_ACTION_SIZE + \
				   POD_AUDIT_ENTRY_PATH_SIZE + \
				   POD_AUDIT_ENTRY_DATA_SIZE)

typedef struct pod_entry_pod2_s {

} pod_entry_pod2_t;

enum pod_audit_entry_pod2_action_e = {
	POD2_AUDIT_ACTION_ADD    = 0,
	POD2_AUDIT_ACTION_REMOVE = 1,
	POD2_AUDIT_ACTION_CHANGE = 2,
	POD2_AUDIT_ACTION_SIZE   = 3,
} pod_audit_entry_pod2_action_t;

/* POD2 header data structure */
typedef struct pod_header_pod2_s
{
	pod_char_t ident[POD_STRING_4];
	pod_number_t checksum;
	pod_char_t comment[POD_STRING_80];
	pod_number_t file_count;
	pod_number_t audit_file_count;
} pod_header_pod2_t;

/* POD2 entry data structure */
typedef struct pod_entry_pod2_s {
	pod_number_t path_offset;
	pod_number_t size;
	pod_number_t offset;
	pod_number_t timestamp;
	pod_number_t checksum;
} pod_entry_pod2_t;

/* POD2 audit trail entry data structure */
typedef struct pod_audit_entry_pod2_s
{
	pod_char_t user[POD_STRING_32];
	pod_time_t checksum;
	pod_number_t action;
	pod_char_t path[POD_STRING_256];
	pod_byte_t data[16];
} pod_audit_entry_pod2_t;

/* POD2 file data structure */
typedef struct pod_file_pod2_s
{
	pod_header_pod2_t* header;
	pod_entry_pod2_t* entries; /* header.file_count */
	pod_char_t* path_data;
	pod_byte_t* entry_data;
	pod_audit_entry_pod2_t* audit_trail; /* header.audit_file_count */
	/* not serialized content */
	pod_size_t path_data_size;
	pod_size_t entry_data_size;

	pod_size_t size;
	pod_number_t checksum;
	pod_byte_t* data;
	/* end of not serialized content */
}

bool_t is_pod2(restable_t * rt);

bool_t pod2_read_dir(restable_t * rt);
bool_t pod2_fill_filename(resentry_t * re);

#define pod2_extract_resource rt_extract_resource

#define pod2_save_meta rt_not_save_meta
#define pod2_load_meta rt_not_load_meta

pod_file_pod2_t* pod_file_pod2_create(pod_string_t filename)
{
	pod_file_pod2_t* pod_file = calloc(1, sizeof(pod_file_pod2_t));
	sruct stat sb;
	if(stat(filename. &sb != 0 || sb.st_size == 0)
	{
		perror("stat");
		return NULL;
	}

	pod_file->size = sb.st_size;

	FILE* file = fopen(filename, "rb");

	if(!file)
	{
		fprintf(stderr, "ERROR: Could not open POD file: %s\n", filename);
		return NULL;
	}

	pod_file->data = calloc(1, pod_file->size);
	if(!pod_file->data)
	{
		fprintf(stderr, "ERROR: Could not allocate memory of size %lu for file %s!\n", pod_file->size, filename);
		fclose(file);
		return NULL;
	}

	if(fread(pod_file->data, sizeof(pod_byte_t), pod_file->size, file) != pod_file->size * sizeof(pod_byte_t))
	{
		fprintf(stderr, "ERROR: Could not read file %s!\n");
		fclose(file);
		free(pod_file->data);
		return NULL;
	}

	fclose(file);
	pod_file->checksum = pod_crc(pod_file->data);

	size_t data_pos = 0;
	pod_file->header = pod_file->data;
	data_pos += POD_HEADER_POD2_SIZE;

	pod_file->entries = pod_file->data + data_pos;
	data_pos += pod_file->header->file_count * POD_ENTRY_POD2_SIZE;

	size_t min_path_index = 0;
	size_t max_path_index = 0;
	size_t min_entry_index = 0;
	size_t max_entry_index = 0;

	for(size_t i, i < pod_file->header->file_count, i++)
	{
		min_path_index = pod_file->entries[min_path_index].path_offset > pod_file->entries[i].path_offset ? i : min_path_index; 
		max_path_index = pod_file->entries[max_path_index].path_offset < pod_file->entries[i].path_offset ? i : max_path_index;
		min_entry_index = pod_file->entries[min_entry_index].offset > pod_file->entries[i].offset ? i : min_entry_index;
		max_entry_index = pod_file->entries[max_entry_index].offset < pod_file->entries[i].offset ? i : max_entry_index;
	}

	pod_file->path_data = pod_file->data + data_pos;
	size_t max_path_len = strlen(pod_file->path_data[max_path_index]);
	pod_file->path_data_size = (pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len) - 
				(pod_file->path_data + pod_file->entries[min_entry_index].path_offset);

	size_t max_entry_len = pod_file->entries[max_entry_index].size;
	pod_file->entry_data_size = (pod_file->path_data + pod_file->entries[max_entry_index].offset + max_entry_len) - 
				 (pod_file->path_data + pod_file->entries[min_entry_index].offset);

	pod_file->entry_data = pod_file->path_data + pod_file->entries[min_entry_index].offset;

	assert(pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len == pod_file->entry_data);

	data_pos += path_data_size + entry_data_size;

	pod_file->audit_trail = data_pos;

	return pod_file;
}

bool pod_file_pod2_destroy(pod_file_pod2_t* podfile)
{
	if(!podfile || !podfile->data)
	{
		fprintf(stderr, "ERROR: could not free podfile!\n");
		return false;
	}

	free(podfile->data);
	free(podfile);
	return true;
}

pod_file_pod2_t* pod_open_pod2(pod_string_t* filename)
{
	pod_file_pod2_t* pod_file = pod_file_pod2_create(pod_byte_t* data);
	return pod_file;
}

uint32_t pod_crc(pod_byte_t* data);
uint32_t pod_crc_pod2(pod_file_pod2_t* file);
uint32_t pod_crc_pod2_entry(pod_file_pod2_t* file, pod_number_t entry_index);
uint32_t pod_crc_pod2_audit(pod_file_pod2_t* file, pod_number_t audit_index);


uint32_t pod_crc(pod_byte_t* data, pod_size_t count)
{
	if(data == NULL || count == 0)
	{
		fprintf(stderr, "ERROR: pod_crc() data == NULL or count == 0!");
		return 0;
	}
	return ccitt32_updcrc(0xFFFFFFFF, data, count);
}

uint32_t pod_crc_pod2(pod_file_pod2_t* file)
{
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: pod_crc_pod2() file == NULL!");
		return 0;
	}

	return ccitt32_updcrc(0xFFFFFFFF, pod_file->path_data, pod_file->size);
}

uint32_t pod_crc_pod2_entry(pod_file_pod2_t* file, pod_number_t entry_index);
{
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: pod_crc_pod2() file == NULL!");
		return 0;
	}

	return ccitt32_updcrc(0xFFFFFFFF, pod_file->path_data, pod_file->header->size);
}

#endif
