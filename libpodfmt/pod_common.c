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
	path0[sep - path ] = POD_PATH_NULL;

	bool ret = pod_rec_mkdir(path0, separator);
       	free(path0);
	return ret;
}

pod_path_t pod_path_system_home()
{
#if defined(__WIN32__) || defined(__WIN64__)
	pod_path_t home = calloc(1, POD_SYSTEM_PATH_SIZE);
	snprintf(home, POD_SYSTEM_PATH_SIZE, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
	pod_path_t home = strdup("/home");
#endif
	return home;
}

pod_path_t pod_path_system_root()
{
#if defined(__WIN32__) || defined(__WIN64__)
	pod_path_t root = calloc(1, POD_SYSTEM_PATH_SIZE);
	snprintf(root, POD_SYSTEM_PATH_SIZE, "%s%c", getenv("SYSTEMDRIVE"), POD_PATH_SEPARATOR);
#else
	pod_path_t root = strdup("/");
#endif
	return root;
}

pod_char_t pod_path_system_drive()
{
	return ((WIN32 || WIN64 ) ? *getenv("SYSTEMDRIVE") : '/');
}

pod_path_t pod_path_posix_to_win32(pod_path_t src, pod_char_t separator, pod_bool_t absolute, pod_char_t drive)
{
	if(src == NULL)
		return NULL;

	absolute = (src[0] == '/') ? true : absolute;
	pod_number_t start = absolute ? (src[0] == '/' ? 2 : 3) : 0;
	pod_number_t size = strlen(src) + start + 1;
	pod_path_t path = calloc(1, size);
	if(!strncpy(&path[start], src, strlen(src)))
	{
		fprintf(stderr, "ERROR: pod_path_posix_to_win32() strncpy() failed");
		free(path);
		return NULL;
	}

	pod_path_t sep = &path[start];

	for(int i = 0; i < size; i++)
	{
		if(path[i] == '/')
		       path[i] = separator;
	}

	path[size] = POD_PATH_NULL;

	if(absolute)
	{
		path[0] = drive;
		path[1] = ':';
		path[2] = separator;
	}

	return &path[0];
}

bool pod_path_is_posix(pod_path_t path)
{
	for(int i = 0; i < strlen(path); i++)
	{
		if(path[i] == '/')
			continue;
		if(isprint(path[i]) != 0 && path[i] != POD_PATH_SEPARATOR)
			continue;
		return false;
	}
	return true;
}


bool pod_path_is_win32(pod_path_t path)
{
	if(isalpha(path[0]) == 0 || path[1] != ':')
		return false;

	for(int i = 0; i < strlen(path); i++)
	{
		if(path[i] == POD_PATH_SEPARATOR)
			continue;
		if(isprint(path[i]) != 0)
			continue;

		return false;
	}

	return true;
}

pod_path_t pod_path_append_win32(pod_path_t a, pod_path_t b)
{
	pod_number_t size_a = strlen(a);
	pod_number_t size_b = strlen(b);
	pod_number_t size = size_a + size_b + 2;
	pod_path_t path = calloc(1, size);
	pod_char_t separator = POD_PATH_SEPARATOR;

	if(!strncpy(path, a, size_a))
		return NULL;

	pod_number_t start = size_a;

	if(path[start - 1] == separator && b[0] == separator)
		start--;
	else if(path[start - 1] != separator && b[0] != separator)
		path[start++] = separator;

	if(!strncpy(&path[start], b, size_b))
		return NULL;

	return path;
}




pod_path_t pod_path_append_posix(pod_path_t a, pod_path_t b)
{
	pod_number_t size_a = strlen(a);
	pod_number_t size_b = strlen(b);
	pod_number_t size = size_a + size_b + 2;
	pod_path_t path = calloc(1, size);
	pod_char_t separator = '/';

	if(!pod_path_is_posix(a) || !pod_path_is_posix(b))
		return NULL;

	if(!strncpy(path, a, size_a))
		return NULL;

	pod_number_t start = size_a;

	if(path[start - 1] == separator && b[0] == separator)
		start--;
	else if(path[start - 1] != separator && b[0] != separator)
		path[start++] = separator;

	if(!strncpy(&path[start], b, size_b))
		return NULL;

	return path;
}

FILE* pod_fopen_mkdir(pod_string_t path, char* mode)
{
	if(path == NULL) { return NULL; }
		pod_char_t *sep = strrchr(path, POD_PATH_SEPARATOR);
	if(sep) {
		pod_char_t *path0 = strdup(path);
		path0[ sep - path ] = POD_PATH_NULL;
		pod_rec_mkdir(path0, POD_PATH_SEPARATOR);
		free(path0);
	}
	return fopen(path, mode);
}
