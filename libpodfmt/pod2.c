#include <assert.h>
#include "pod_common.h"
#include "pod2.h"

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
	if(file == NULL || file->path_data == NULL)
	{
		fprintf(stderr, "ERROR: pod_crc_pod2() file == NULL!");
		return 0;
	}

	return ccitt32_updcrc(0xFFFFFFFF, file->entry_data, file->entry_data_size);
}

uint32_t pod_crc_pod2_entry(pod_file_pod2_t* file, pod_number_t entry_index)
{
	if(file == NULL || file->entry_data == NULL)
	{
		fprintf(stderr, "ERROR: pod_crc_pod2() file == NULL!");
		return 0;
	}

	return ccitt32_updcrc(0xFFFFFFFF, file->entry_data + file->entries[entry_index].offset, file->entries[entry_index].size);
}


bool is_pod2(char* ident)
{
  return (POD2 == pod_type(ident) >= 0);
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

pod_file_pod2_t* pod_file_pod2_create(pod_string_t filename)
{
	pod_file_pod2_t* pod_file = calloc(1, sizeof(pod_file_pod2_t));
	struct stat sb;
	if(stat(filename, &sb) != 0 || sb.st_size == 0)
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
	pod_file->checksum = pod_crc(pod_file->data, pod_file->size);

	size_t data_pos = 0;
	pod_file->header = (pod_header_pod2_t*)pod_file->data;
	data_pos += POD_HEADER_POD2_SIZE;

	pod_file->entries = (pod_entry_pod2_t*)(pod_file->data + data_pos);
	data_pos += pod_file->header->file_count * POD_DIR_ENTRY_POD2_SIZE;

	size_t min_path_index = 0;
	size_t max_path_index = 0;
	size_t min_entry_index = 0;
	size_t max_entry_index = 0;

	for(size_t i; i < pod_file->header->file_count; i++)
	{
		min_path_index = pod_file->entries[min_path_index].path_offset > pod_file->entries[i].path_offset ? i : min_path_index; 
		max_path_index = pod_file->entries[max_path_index].path_offset < pod_file->entries[i].path_offset ? i : max_path_index;
		min_entry_index = pod_file->entries[min_entry_index].offset > pod_file->entries[i].offset ? i : min_entry_index;
		max_entry_index = pod_file->entries[max_entry_index].offset < pod_file->entries[i].offset ? i : max_entry_index;
	}

	pod_file->path_data = pod_file->data + data_pos;
	size_t max_path_len = strlen(pod_file->path_data + pod_file->entries[max_path_index].path_offset);
	pod_file->path_data_size = (pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len) - 
				(pod_file->path_data + pod_file->entries[min_entry_index].path_offset);

	size_t max_entry_len = pod_file->entries[max_entry_index].size;
	pod_file->entry_data_size = (pod_file->path_data + pod_file->entries[max_entry_index].offset + max_entry_len) - 
				 (pod_file->path_data + pod_file->entries[min_entry_index].offset);

	pod_file->entry_data = pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len;

	assert((pod_byte_t*)(pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len) == (pod_byte_t*)pod_file->entry_data);


	data_pos += pod_file->path_data_size + pod_file->entry_data_size;

	pod_file->audit_trail = (pod_audit_entry_pod2_t*) pod_file->data + data_pos;

	return pod_file;
}
