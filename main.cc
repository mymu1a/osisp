#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>
#include <cerrno>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "gengetopt/cmdline.h"



/*
struct dirent {
	ino_t          d_ino;				// inode number
	off_t          d_off;				// offset to the next dirent
	unsigned short d_reclen;			// length of this record
	unsigned char  d_type;				// type of file; not supported by all file system types
	char           d_name[256];			// filename
};
*/

enum { FMT_LONG, FMT_SHORT };
static int format_type = FMT_LONG;

static int eval_realpath(const char* given)
{
	char realname[_POSIX_PATH_MAX];
	int rc = 0;

	if (realpath(given, realname) == 0)
	{
		rc = -1;
//		err_sysrem("failed to resolve real path name for %s\n", given);
	}
	else if (format_type == FMT_SHORT)
		printf("%s\n", realname);
	else
		printf("%s %s\n", given, realname);

	return(rc);
}


int main_2(void) {

	eval_realpath("./");
	eval_realpath("..");
	eval_realpath("~/osisp");
	eval_realpath("/home/artur/osisp");
	eval_realpath("/home/artur/osisp/");

	char buf[PATH_MAX]; /* PATH_MAX incudes the \0 so +1 is not required */
	char* res = realpath("~/osisp", buf);
	if (res) { // or: if (res != NULL)
		printf("This source is at %s.\n", buf);
	}
	else {
		char* errStr = strerror(errno);
		printf("error string: %s\n", errStr);

		perror("realpath");
		exit(EXIT_FAILURE);
	}
	return 0;
}


int main(int argc, char** argv)
{
	//=== parse command line options ===
	struct gengetopt_args_info	config;

	if (cmdline_parser(argc, argv, &config) != 0)
	{
		exit(1);
	}
	bool all_flag = false;

	if (config.links_flag == false && config.dirs_flag == false && config.files_flag == false)
	{
		all_flag = true;
	}
	//=== open directory stream ===
	char	pathDir_[] = "./";
	char*	pathDir = pathDir_;
	DIR*	pDir;

	pDir = opendir(pathDir);
	if (pDir == NULL)
	{
		printf("\nError: %d (line: %d)\n", errno, __LINE__);
		return 1;
	}
	//=== print parameters ===
	bool print_slash = false;

	if (config.inputs_num > 0)
	{
		pathDir = config.inputs[0];

		size_t size = strlen(pathDir);
		if (pathDir[size - 1] != '/')
		{
			print_slash = true;
		}
	}
	//=== loop through the directory stream ===
	struct dirent* pEntry;

	while (( pEntry = readdir(pDir) ) != NULL)
	{
		if (strcmp(pEntry->d_name, ".") == 0 || strcmp(pEntry->d_name, "..") == 0)
		{
			continue;
		}
		if (pEntry->d_type == DT_DIR && all_flag == false && config.dirs_flag == false)
		{
			continue;
		}
		if (pEntry->d_type == DT_REG && all_flag == false && config.files_flag == false)
		{
			continue;
		}
		if (pEntry->d_type == DT_LNK && all_flag == false && config.links_flag == false)
		{
			continue;
		}

		printf("%s", pathDir);
		if (print_slash == true)
		{
			printf("/");
		}
		printf("%s\n", pEntry->d_name);
	}
	// close the directory stream
	closedir(pDir);

	return 0;
}
