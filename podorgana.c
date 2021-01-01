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

		pod_file_pod2_t* pod2 = pod_file_pod2_create(src);
		if(pod2 == NULL)
		{
			fprintf(stderr, "ERROR: cannot create pod2 file!\n");
			exit(EXIT_FAILURE);
		}

		pod_file_pod2_print(pod2);
		

		if(!pod_file_pod2_write(pod2, "out.pod"))
		{
			fprintf(stderr, "ERROR: cannot write out.pod!\n");
			pod_file_pod2_destroy(pod2);
			exit(EXIT_FAILURE);
		}

		
		pod_path_t dst = pod_path_append_posix(argv[2], pod_path_trim(src));
		printf("dst: %s trimmed: %s\n", src, dst);
		if(!pod_file_pod2_extract(pod2, dst, false))
		{
			fprintf(stderr, "ERROR: pod_file_pod2_extract(%s) failed!\n", dst);
			pod_file_pod2_destroy(pod2);
			exit(EXIT_FAILURE);
		}

		pod_file_pod2_destroy(pod2);
		free(src);
	}
	else
	{
		print_usage(argc,argv);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

