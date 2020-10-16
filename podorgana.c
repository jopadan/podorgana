#include "libpodfmt.h"

bool print_usage(int argc, char** argv)
{
	fprintf(stderr, "%s <pod file name>\n", argv[0]);
	return true;
}

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		pod_file_pod2_t* pod2 = pod_file_pod2_create(argv[1]);
		if(pod2 == NULL)
		{
			fprintf(stderr, "ERROR: cannot create pod2 file!\n");
			exit(EXIT_FAILURE);
		}
		pod_file_pod2_print(pod2);
		pod_file_pod2_destroy(pod2);
	}
	else
	{
		print_usage(argc,argv);
		exit(EXIT_FAILURE);
	}


	exit(EXIT_SUCCESS);
}

