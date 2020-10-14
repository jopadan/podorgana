#include "pod_common.h"

const char POD_IDENT[POD_IDENT_TYPE_SIZE][POD_IDENT_SIZE + 1] = {
	"\0POD1", "POD2\0", "POD3\0",
       	"POD4\0", "POD5\0", "POD6\0", "dtxe\0"
};

const ssize_t POD_HEADER_SIZE[POD_IDENT_TYPE_SIZE] =
{
	POD_HEADER_POD1_SIZE,
	POD_HEADER_POD2_SIZE,
	POD_HEADER_POD3_SIZE,
	POD_HEADER_POD4_SIZE,
	POD_HEADER_POD5_SIZE,
	POD_HEADER_POD6_SIZE,
	POD_HEADER_EPD_SIZE,
};

const ssize_t POD_DIR_ENTRY_SIZE[POD_IDENT_TYPE_SIZE] =
{
	POD_DIR_ENTRY_POD1_SIZE,
	POD_DIR_ENTRY_POD2_SIZE,
	POD_DIR_ENTRY_POD3_SIZE,
	POD_DIR_ENTRY_POD4_SIZE,
	POD_DIR_ENTRY_POD5_SIZE,
	POD_DIR_ENTRY_POD6_SIZE,
	POD_DIR_ENTRY_EPD_SIZE,
};


int pod_type(char* ident)
{
	for(int i = 0; i < POD_IDENT_TYPE_SIZE; i++)
	{
		const char * pod_ident = &POD_IDENT[i][0];
		if (strncmp(ident, pod_ident, strlen(pod_ident)) == 0)
			return i;
	}
  	return POD1;
}

bool is_pod(char* ident)
{
  return (POD_IDENT_TYPE_SIZE > pod_type(ident) >= 0);
}

pod_string_t pod_type_to_file_ext(int pod_type)
{
	switch(pod_type)
	{
		case EPD:
			return "EPD";
		default:
			return "POD";
	}

	return NULL;
}
