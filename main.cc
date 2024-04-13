#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>
#include <cerrno>

#include <limits.h>
#include <stdlib.h>
#include <string.h>


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


int main(void) {

	eval_realpath("./");
	eval_realpath("..");
	eval_realpath("~/osisp");

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

int main_1()
{
	char pathDir[] = "~/osisp";
	char pathResolved[PATH_MAX];

	if (realpath(pathDir, pathResolved) == NULL)
	{
		printf("\nError: %d (line: %d)\n", errno, __LINE__);
		return 1;
	}
	return 0;
}

int main_2()
{
	printf("hello");
	char pathDir[] = "~/osisp";
///	char pathDir[] = "./";

	// pointer to the directory stream
	DIR*	pDir;


	// open a directory stream
	pDir = opendir(pathDir);

	if (pDir == NULL)
	{
		char* resolved_path = NULL;

		if (realpath(pathDir, resolved_path) == NULL)
		{
			printf("\nError: %d (line: %d)\n", errno, __LINE__);
			return 1;
		}
		pDir = opendir(resolved_path);
		free(resolved_path);

		if (pDir == NULL)
		{
			printf("\nError: %d (line: %d)\n", errno, __LINE__);
			return 1;
		}
	}
	struct dirent* pEntry;

	while (( pEntry = readdir(pDir) ) != NULL)
	{
		printf("%s\n", pEntry->d_name);
	}


	// close the directory stream
	closedir(pDir);

	return 0;
}
