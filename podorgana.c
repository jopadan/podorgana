#include "libpodfmt/libpodfmt.h"


bool print_usage(int argc, char** argv)
{
	fprintf(stderr, "%s <pod file name>\n", argv[0]);
	return true;
}

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		pod_path_t src = strdup(argv[1]);
		if(src == NULL)
			fprintf(stderr, "ERROR: path == NULL!");

		printf("native path: %s\n", src);

		/* open POD5 file copied from argv[1] */
		pod_file_pod5_t* pod5 = pod_file_pod5_create(src);
		if(pod5 == NULL)
		{
			fprintf(stderr, "ERROR: cannot create pod5 file!\n");
			exit(EXIT_FAILURE);
		}

		pod_file_pod5_print(pod5);
		
/*
		if(!pod_file_pod5_write(pod5, "out.pod"))
		{
			fprintf(stderr, "ERROR: cannot write out.pod!\n");
			pod_file_pod5_destroy(pod5);
			exit(EXIT_FAILURE);
		}

		
		pod_path_t dst = pod_path_append_posix(argv[2], pod_path_trim(src));
		printf("dst: %s trimmed: %s\n", src, dst);
		if(!pod_file_pod5_extract(pod5, dst, false))
		{
			fprintf(stderr, "ERROR: pod_file_pod5_extract(%s) failed!\n", dst);
			pod_file_pod5_destroy(pod5);
			exit(EXIT_FAILURE);
		}
*/
		pod_file_pod5_destroy(pod5);
		free(src);
	}
	else
	{
		print_usage(argc,argv);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

