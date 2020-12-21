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
		pod_path_t path = pod_path_to_system_path(argv[1], POD_PATH_SEPARATOR);
		if(path == NULL)
			fprintf(stderr, "ERROR: path == NULL!");
		printf("native path: %s\n", path);
		pod_file_pod2_t* pod2 = pod_file_pod2_create(path);
		if(pod2 == NULL)
		{
			fprintf(stderr, "ERROR: cannot create pod2 file!\n");
			exit(EXIT_FAILURE);
		}

		pod_file_pod2_print(pod2);
		
		/*	
		if(!pod_file_pod2_write(pod2, "foo.pod"))
		{
			fprintf(stderr, "ERROR: cannot write foo.pod!\n");
			pod_file_pod2_destroy(pod2);
			exit(EXIT_FAILURE);
		}
		
		
		if(!pod_directory_create(path, POD_PATH_SEPARATOR))
		{
			fprintf(stderr, "ERROR: pod_directory_failed!\n");
		}
		pod_file_pod2_extract(pod2, "/pod/foo");
		*/
		pod_file_pod2_destroy(pod2);
		free(path);
	}
	else
	{
		print_usage(argc,argv);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

