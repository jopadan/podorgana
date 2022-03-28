#include <stdlib.h>
#include <stdio.h>

#include "libpodfmt/libpodfmt.h"


bool print_usage(int argc, char** argv)
{
	fprintf(stderr, "%s <pod file name>\n", argv[0]);
	return true;
}

int main(int argc, char** argv)
{
	pod_file_type_t pod = { NULL };
	pod_ident_type_t pod_type = 0;
	int exit_code = EXIT_SUCCESS;

	if(argc > 1)
	{
		pod_path_t src = strdup(argv[1]);
		if(src == NULL)
			fprintf(stderr, "ERROR: path == NULL!");
		pod_type = pod_type_peek(src);
		printf("native path: %s type: %d\n", src, pod_type);
		pod = pod_file_create(src);
		pod_type = pod_file_typeid(pod);
		printf("native path: %s type: %d\n", src, pod_type);
		if(pod.pod1 == NULL)
		{
			fprintf(stderr, "ERROR: cannot create POD file!\n");
			exit_code = EXIT_FAILURE;

		}
		else
		{
			pod_file_print(pod);
			pod_file_delete(pod);
		}
/*		
		if(!pod_file_pod3_write(pod3, "out.pod"))
		{
			fprintf(stderr, "ERROR: cannot write out.pod!\n");
			pod_file_pod3_destroy(pod3);
			exit(EXIT_FAILURE);
		}

		
		pod_path_t dst = pod_path_append_posix(argv[2], pod_path_trim(src));
		printf("dst: %s trimmed: %s\n", src, dst);
		if(!pod_file_pod3_extract(pod3, dst, false))
		{
			fprintf(stderr, "ERROR: pod_file_pod3_extract(%s) failed!\n", dst);
			pod_file_pod3_destroy(pod3);
			exit(EXIT_FAILURE);
		}
*/
		free(src);
	}
	else
		print_usage(argc,argv);

	exit(exit_code);
}

