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

	return ccitt32_updcrc(0xFFFFFFFF, file->data + POD_IDENT_SIZE + POD_HEADER_CHECKSUM_SIZE, file->size - POD_IDENT_SIZE - POD_HEADER_CHECKSUM_SIZE);
}

uint32_t pod_crc_pod2_entry(pod_file_pod2_t* file, pod_number_t entry_index)
{
	if(file == NULL || file->entry_data == NULL)
	{
		fprintf(stderr, "ERROR: pod_crc_pod2() file == NULL!");
		return 0;
	}

	return ccitt32_updcrc(0xFFFFFFFF, file->data + file->entries[entry_index].offset, file->entries[entry_index].size);
}
 

bool is_pod2(char* ident)
{
  return (POD2 == pod_type(ident) >= 0);
}

bool pod_file_pod2_print(pod_file_pod2_t* podfile)
{
	if(podfile == NULL)
	{
		fprintf(stderr, "ERROR: pod_file_pod2_print() podfile == NULL\n");
		return false;
	}
	for(pod_number_t i = 0; i < podfile->header->file_count; i++)
	{
		pod_entry_pod2_t* entry = &podfile->entries[i];
		pod_char_t* name = podfile->path_data + podfile->entries[i].path_offset;
		printf("entry: %u name: %s path_offset %u size: %u offset: %u timestamp: %u recorded checksum: %u calculated checksum: %u\n", i, name, entry->path_offset, entry->size, entry->offset, entry->timestamp, entry->checksum, pod_crc_pod2_entry(podfile, i));
	}
	fprintf(stdout,"filename           : %s\nformat             : %s\ncomment            : %s\nrecorded checksum  : %u\ncalculated checksum: %u\nfile checksum      : %u\nsize               : %u\nfile entries       : %u\naudit entries      : %u\n", podfile->filename, pod_type_str(pod_type(podfile->header->ident)), podfile->header->comment, podfile->header->checksum, pod_crc_pod2(podfile), podfile->checksum, podfile->size, podfile->header->file_count, podfile->header->audit_file_count);
	return true;
}

bool pod_file_pod2_destroy(pod_file_pod2_t* podfile)
{
	if(!podfile || !podfile->data)
	{
		fprintf(stderr, "ERROR: could not free podfile!\n");
		return false;
	}

	free(podfile->data);
	free(podfile->filename);
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

	pod_file->filename = calloc(1, strlen(filename));
	pod_file->filename = strcpy(pod_file->filename, filename);
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

	if(fread(pod_file->data, POD_BYTE_SIZE, pod_file->size, file) != pod_file->size * POD_BYTE_SIZE)
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

	pod_number_t min_path_index = 0;
	pod_number_t max_path_index = 0;
	pod_number_t min_entry_index = 0;
	pod_number_t max_entry_index = 0;

	for(pod_number_t i = 0; i < pod_file->header->file_count; i++)
	{
		if(pod_file->entries[i].path_offset < pod_file->entries[min_path_index].path_offset)
		{
			min_path_index = i;
		}
		if(pod_file->entries[i].path_offset > pod_file->entries[max_path_index].path_offset)
		{
			max_path_index = i;
		}
		if(pod_file->entries[i].offset < pod_file->entries[min_entry_index].offset)
		{
			min_entry_index = i;
		}
		if(pod_file->entries[i].offset > pod_file->entries[max_entry_index].offset)
		{
			max_entry_index = i;
		}
	}


	pod_file->path_data = (pod_char_t*) (pod_file->data + data_pos);
	size_t max_path_len = strlen(pod_file->path_data + pod_file->entries[max_path_index].path_offset);

	pod_file->path_data_size = (pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len) - 
				(pod_file->path_data + pod_file->entries[min_entry_index].path_offset);

	size_t max_entry_len = pod_file->entries[max_entry_index].size;
	pod_file->entry_data_size = (pod_file->path_data + pod_file->entries[max_entry_index].offset + max_entry_len) - 
				 (pod_file->path_data + pod_file->entries[min_entry_index].offset);

	pod_file->entry_data = pod_file->path_data + pod_file->entries[min_entry_index].offset;

	data_pos += pod_file->path_data_size + pod_file->entry_data_size;

	pod_file->audit_trail = (pod_audit_entry_pod2_t*) pod_file->data + data_pos;
	return pod_file;
}
