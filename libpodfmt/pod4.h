#ifndef _POD4_H
#define _POD4_H

#include "pod_common.h"

/* POD4 header data structure */
typedef struct pod_header_pod4_s
{
	pod_char_t ident[POD_HEADER_IDENT_SIZE];
	pod_number_t checksum;
	pod_char_t comment[POD_HEADER_COMMENT_SIZE];
	pod_number_t file_count;
	pod_number_t audit_file_count;
	pod_number_t revision;
	pod_number_t priority;
	pod_char_t author[POD_HEADER_AUTHOR_SIZE];
	pod_char_t copyright[POD_HEADER_COPYRIGHT_SIZE];
	pod_number_t index_offset;
	pod_number_t unknown10c;
	pod_number_t size_index;
	pod_number_t unknown114;
	pod_number_t unknown118;
	pod_number_t unknown11C;
} pod_header_pod4_t;

/* POD4 entry data structure */
typedef struct pod4_entry_s {
	pod_number_t path_offset;
	pod_number_t size;
	pod_number_t offset;
	pod_number_t uncompressed;
	pod_number_t compression_level;
	pod_number_t timestamp;
	pod_number_t checksum;
} pod4_entry_t;


bool is_pod4(char* ident);

#endif
