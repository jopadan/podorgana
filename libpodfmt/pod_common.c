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


pod_ident_type_t pod_type(char* ident)
{
	for(int i = 0; i < POD_IDENT_TYPE_SIZE; i++)
	{
		if (strncmp(ident, POD_IDENT[i], POD_IDENT_SIZE) == 0)
			return i;
	}
  	return POD1;
}

const char* pod_type_str(pod_ident_type_t type)
{
	return POD_IDENT[type];
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

pod_string_t pod_ctime(pod_time_t* time32)
{
	struct tm tm;
	errno_t ret = _localtime32_s(&tm, time32);
	if(ret != 0)
	{
		fprintf(stderr, "%s\n", strerror(ret));
		return 0;
	}
	__time64_t time64 = _mktime64(&tm);
	char* str = _ctime64(&time64);
	str[strcspn(str, "\n")] = '\0';

	return  str;
}


bool pod_rec_mkdir(pod_string_t path, char separator)
{
	pod_char_t *sep = strrchr(path, POD_PATH_SEPARATOR);
	if(sep != NULL)
	{
		*sep = '\0';
		if(!pod_rec_mkdir(path, separator))
		{
			*sep = separator;
			fprintf(stderr,"ERROR: pod_rec_mkdir(%s) failed: %s\n", path, strerror(errno));
			return false;
		}
		*sep = separator;
	}

	if(mkdir(path) && errno != EEXIST)
	{
		fprintf(stderr,"ERROR: mkdir(%s): %s\n", path, strerror(errno));
		return false;
	}
	return true;
}

bool pod_directory_create(pod_string_t path, char separator)
{
	if(path == NULL)
		return false;

	pod_char_t *sep = strrchr(path, separator);

	if(sep == NULL)
		return false;

	pod_char_t *path0 = strdup(path);
	path0[sep - path ] = '\0';

	bool ret = pod_rec_mkdir(path0, separator);
       	free(path0);
	return ret;
}

pod_path_t pod_path_system_home()
{
#if defined(__WIN32__) || defined(__WIN64__)
#if defined (__MSYS__) 
	pod_path_t home = cygwin_create_path(CCP_POSIX_TO_WIN_A | CCP_ABSOLUTE, "/home");
	if(home == NULL)
		perror("cygwin_conv_path");
#else
	pod_path_t home = calloc(0, POD_SYSTEM_PATH_SIZE);
	snprintf(home, POD_SYSTEM_PATH_SIZE, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#endif
#else
	pod_path_t home = strdup("/home");
#endif
	return home;
}

pod_path_t pod_path_system_root()
{
#if defined(__WIN32__) || defined(__WIN64__)
#if defined (__MSYS__)
	pod_path_t root = cygwin_create_path(CCP_POSIX_TO_WIN_A | CCP_ABSOLUTE, "/");
	if(root == NULL)
		perror("cygwin_conv_path");
#else
	pod_path_t root = getenv("SYSTEMDRIVE");
#endif
#else
	pod_path_t root = strdup("/");
#endif
	return root;
}

pod_path_t pod_path_append(pod_path_t a, pod_path_t b)
{
	ssize_t size_a = strlen(a);
	ssize_t size_b = strlen(b);
	ssize_t size = size_a + size_b + 1;
	pod_path_t path = calloc(0, size);
	pod_path_t dst = strncpy(path, a, size_a);
	strncpy(&dst[strlen(dst)], b, size_b);
	return dst;
}

FILE* pod_fopen_mkdir(pod_string_t path, char* mode)
{
	if(path == NULL) { return NULL; }
	pod_char_t *sep = strrchr(path, POD_PATH_SEPARATOR);
	if(sep) {
		pod_char_t *path0 = strdup(path);
		path0[ sep - path ] = '\0';
		pod_rec_mkdir(path0, POD_PATH_SEPARATOR);
		free(path0);
	}
	return fopen(path, mode);
}
