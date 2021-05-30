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
	int exit_code = EXIT_SUCCESS;

	if(argc > 1)
	{
		pod_path_t src = strdup(argv[1]);
		if(src == NULL)
			fprintf(stderr, "ERROR: path == NULL!");

		printf("native path: %s type: %d\n", src, pod_type_peek(src));
		switch(pod_type_peek(src))
		{
			case POD1:
				pod.pod1 = pod_file_pod1_create(src);
				if(pod.pod1 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod1 file!\n");
					exit_code = EXIT_FAILURE;
				}
				pod_file_pod1_print(pod.pod1);
				pod_file_pod1_destroy(pod.pod1);
				break;
			case POD2:
				pod.pod2 = pod_file_pod2_create(src);
				if(pod.pod2 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod2 file!\n");
					exit_code = EXIT_FAILURE;
				}
				pod_file_pod2_print(pod.pod2);
				pod_file_pod2_destroy(pod.pod2);
				break;
			case POD3:
				/* open POD3 file copied from argv[1] */
				pod.pod3 = pod_file_pod3_create(src);
				if(pod.pod3 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod3 file!\n");
					exit_code = EXIT_FAILURE;
				}
				pod_file_pod3_print(pod.pod3);
				pod_file_pod3_destroy(pod.pod3);
				break;
			case POD4:
				/* open POD3 file copied from argv[1] */
				pod.pod4 = pod_file_pod4_create(src);
				if(pod.pod4 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod4 file!\n");
					exit_code = EXIT_FAILURE;
				}

				pod_file_pod4_print(pod.pod4);
				pod_file_pod4_destroy(pod.pod4);
				break;
			case POD5:
				/* open POD5 file copied from argv[1] */
				pod.pod5 = pod_file_pod5_create(src);
				if(pod.pod5 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod5 file!\n");
					exit_code = EXIT_FAILURE;
				}
				pod_file_pod5_print(pod.pod5);
				pod_file_pod5_destroy(pod.pod5);
				break;
			case POD6:
				/* open POD6 file copied from argv[1] */
				pod.pod6 = pod_file_pod6_create(src);
				if(pod.pod6 == NULL)
				{
					fprintf(stderr, "ERROR: cannot create pod6 file!\n");
					exit_code = EXIT_FAILURE;
				}
				pod_file_pod6_print(pod.pod6);
				pod_file_pod6_destroy(pod.pod6);
				break;
			case EPD:
				/* open EPD file copied from argv[1] */
				pod.epd = pod_file_epd_create(src);
				if(pod.epd == NULL)
				{
					fprintf(stderr, "ERROR: cannot create epd file!\n");
					exit_code = EXIT_FAILURE;
				}

				pod_file_epd_print(pod.epd);
				pod_file_epd_destroy(pod.epd);
				break;
			default:
				fprintf(stderr, "ERROR: unknown file format!\n");
				exit_code = EXIT_FAILURE;
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
		print_usage(argc,argv);

	exit(exit_code);
}

