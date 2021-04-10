#include "libpodfmt/libpodfmt.h"


bool print_usage(int argc, char** argv)
{
	fprintf(stderr, "%s <pod file name>\n", argv[0]);
	return true;
}

int main(int argc, char** argv)
{
	pod_file_type_t pod = { NULL };

	if(argc > 1)
	{
		pod_path_t src = strdup(argv[1]);
		if(src == NULL)
			fprintf(stderr, "ERROR: path == NULL!");

		printf("native path: %s type: %d\n", src, pod_type_peek(src));
		switch(pod_type_peek(src))
		{
			case POD1:
				break;
			case POD2:
				break;
			case POD3:
				/* open POD3 file copied from argv[1] */
				pod.pod3 = pod_file_pod3_create(src);
				if(pod.pod3 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod3 file!\n");
					exit(EXIT_FAILURE);
				}

				pod_file_pod3_print(pod.pod3);
				pod_file_pod3_destroy(pod.pod3);
				break;
			case POD4:
				break;
			case POD5:
				/* open POD5 file copied from argv[1] */
				pod.pod5 = pod_file_pod5_create(src);
				if(pod.pod5 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod5 file!\n");
					exit(EXIT_FAILURE);
				}

				pod_file_pod5_print(pod.pod5);
				pod_file_pod5_destroy(pod.pod5);
				break;
			case POD6:
				break;
			case EPD:
				break;
			default:
				break;
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
	{
		print_usage(argc,argv);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

