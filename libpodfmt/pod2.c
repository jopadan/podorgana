#include <assert.h>
#include "pod_common.h"
#include "pod2.h"

const char pod_audit_action_string[POD2_AUDIT_ACTION_SIZE][8] = { "Add", "Remove", "Change" };

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

bool pod_audit_entry_pod2_print(pod_audit_entry_pod2_t* audit)
{
	if(audit == NULL)
	{
		fprintf(stderr, "ERROR: pod_audit_entry_pod2_print(audit == NULL)!\n");
		return false;
	}
	/*
	switch(pod_file->audit_trail[i].action)
	{
		case POD_AUDIT_ACTION_ADD:
			pod_file->audit_trail[i].new_data;
			break;
		case POD_AUDIT_ACTION_REMOVE:
			pod_file->audit_trail[i].old_data;
			break;
		case POD_AUDIT_ACTION_CHANGE:
			pod_file->audit_trail[i].old_data; 
			pod_file->audit_trail[i].new_data;
			break;
		default:
			break;
		}
	}
	*/
	printf("user: %s timestamp: %d action: %s path: %s old_timestamp: %s old_size: %s new_timestamp: %s new_size: %s\n", audit->user, pod_ctime(&audit->timestamp), pod_audit_action_string[audit->action], audit->path, pod_ctime(&audit->old_timestamp), audit->old_size, pod_ctime(&audit->new_timestamp), audit->new_size);
	return true;
}

bool pod_file_pod2_print(pod_file_pod2_t* pod_file)
{
	if(pod_file == NULL)
	{
		fprintf(stderr, "ERROR: pod_file_pod2_print() podfile == NULL\n");
		return false;
	}
	for(pod_number_t i = 0; i < pod_file->header->file_count; i++)
	{
		pod_entry_pod2_t* entry = &pod_file->entries[i];
		pod_char_t* name = pod_file->path_data + pod_file->entries[i].path_offset;
		printf("entry: %u name: %s path_offset %u size: %u offset: %u timestamp: %s recorded checksum: %u calculated checksum: %u\n", i, name, entry->path_offset, entry->size, entry->offset, pod_ctime(&entry->timestamp), entry->checksum, pod_crc_pod2_entry(pod_file, i));
	}
	for(int i = 0; i < pod_file->header->audit_file_count; i++)
	{
		if(!pod_audit_entry_pod2_print(&pod_file->audit_trail[i]))
		{
			fprintf(stderr, "ERROR: pod_audit_entry_pod2_print() failed!");
			pod_file_pod2_destroy(pod_file);
			return NULL;
		}
	}	
	fprintf(stdout,"filename           : %s\nformat             : %s\ncomment            : %s\nrecorded checksum  : %u\ncalculated checksum: %u\nfile checksum      : %u\nsize               : %u\nfile entries       : %u\naudit entries      : %u\n", pod_file->filename, pod_type_str(pod_type(pod_file->header->ident)), pod_file->header->comment, pod_file->header->checksum, pod_crc_pod2(pod_file), pod_file->checksum, pod_file->size, pod_file->header->file_count, pod_file->header->audit_file_count);
	return true;
}

bool pod_file_pod2_destroy(pod_file_pod2_t* podfile)
{
	if(!podfile)
	{
		fprintf(stderr, "ERROR: could not free podfile == NULL!\n");
		return false;
	}

	if(podfile->data)
		free(podfile->data);
	if(podfile->filename);
		free(podfile->filename);
	if(podfile)
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
		pod_file_pod2_destroy(pod_file);
		return NULL;
	}

	if(fread(pod_file->data, POD_BYTE_SIZE, pod_file->size, file) != pod_file->size * POD_BYTE_SIZE)
	{
		fprintf(stderr, "ERROR: Could not read file %s!\n");
		fclose(file);
		pod_file_pod2_destroy(pod_file);
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
	size_t max_path_len = strlen(pod_file->path_data + pod_file->entries[max_path_index].path_offset) + 1;

	pod_file->path_data_size = (pod_file->path_data + pod_file->entries[max_path_index].path_offset + max_path_len) - 
				(pod_file->path_data + pod_file->entries[min_entry_index].path_offset);

	size_t max_entry_len = pod_file->entries[max_entry_index].size;
	pod_file->entry_data_size = (pod_file->data + pod_file->entries[max_entry_index].offset + max_entry_len) - 
				 (pod_file->data + pod_file->entries[min_entry_index].offset);

	pod_file->entry_data = pod_file->data + pod_file->entries[min_entry_index].offset;

	data_pos += pod_file->path_data_size + pod_file->entry_data_size;

	pod_file->audit_trail = (pod_audit_entry_pod2_t*)(pod_file->data + data_pos);

	return pod_file;
}

bool pod_file_pod2_add_entry(pod_file_pod2_t* pod_file, pod_entry_pod2_t* entry, pod_byte_t* data)
{
	if(pod_file == NULL || entry == NULL || data == NULL)
	{
		fprintf(stderr, "ERROR: pod_file, entry or data equals NULL!\n");
		return false;
	}

	
	return true;
}

bool pod_file_pod2_write(pod_file_pod2_t* pod_file, pod_string_t filename)
{
	if(pod_file == NULL || filename == NULL)
	{
		fprintf(stderr, "ERROR: pod_file or filename equals NULL!\n");
		return false;
	}
	FILE *file = fopen(filename, "wb");

	/* write pod_header_pod2_t */
	if(fwrite(pod_type_str(POD2), POD_CHAR_SIZE, POD_IDENT_SIZE, file) != POD_IDENT_SIZE)
	{
		fprintf(stderr, "ERROR: writing POD2 ident string!\n");
		fclose(file);
		return false;
	}

	/* calculate new checksum and write it */
	pod_file->header->checksum = pod_crc_pod2(pod_file);
	if(fwrite(&pod_file->header->checksum, POD_HEADER_CHECKSUM_SIZE, 1, file) != 1)
	{
		fprintf(stderr, "ERROR: writing checksum!\n");
		fclose(file);
		return false;
	}
	/* write comment string */
	if(fwrite(pod_file->header->comment, 1, POD_COMMENT_SIZE, file) != POD_COMMENT_SIZE)
	{
		fprintf(stderr, "ERROR: writing comment string!\n");
		fclose(file);
		return false;
	}
	/* write file count */
	if(fwrite(&pod_file->header->file_count, POD_NUMBER_SIZE, 1, file) != 1)
	{
		fprintf(stderr, "ERROR: writing file count!\n");
		fclose(file);
		return false;
	}
	/* write audit file count */
	if(fwrite(&pod_file->header->audit_file_count, POD_NUMBER_SIZE, 1, file) != 1)
	{
		fprintf(stderr, "ERROR: writing audit file count!\n");
		fclose(file);
		return false;
	}
	/* write entries */
	if(fwrite(pod_file->entries, POD_DIR_ENTRY_POD2_SIZE, pod_file->header->file_count, file) != pod_file->header->file_count)
	{
		fprintf(stderr, "ERROR: writing entries!\n");
		fclose(file);
		return false;
	}
	/* write path data */
	if(fwrite(pod_file->path_data, 1, pod_file->path_data_size, file) != pod_file->path_data_size)
	{
		fprintf(stderr, "ERROR: writing path data!\n");
		fclose(file);
		return false;
	}
	/* write entry data */
	if(fwrite(pod_file->entry_data, 1, pod_file->entry_data_size, file) != pod_file->entry_data_size)
	{
		fprintf(stderr, "ERROR: writing entry data!\n");
		fclose(file);
		return false;
	}
	/* write audit trail */

	if(fwrite(pod_file->audit_trail, POD_AUDIT_ENTRY_POD2_SIZE, pod_file->header->audit_file_count, file) != pod_file->header->audit_file_count)
	{
		fprintf(stderr, "ERROR: writing audit trail!\n");
		fclose(file);
		return false;
	}

	fclose(file);
	return true;	
}
