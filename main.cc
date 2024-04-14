#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>
#include <cerrno>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "gengetopt/cmdline.h"


void print(struct dirent* pEntry, char* pathDir, bool printSlash);
void readdir(DIR* pDir, char* pathDir, bool printSlash);
void scandir(char* pathDir, bool printSlash);

/*
struct dirent {
	ino_t          d_ino;				// inode number
	off_t          d_off;				// offset to the next dirent
	unsigned short d_reclen;			// length of this record
	unsigned char  d_type;				// type of file; not supported by all file system types
	char           d_name[256];			// filename
};
*/


struct gengetopt_args_info	config;
bool all_flag = false;

int main(int argc, char** argv)
{
	//=== parse command line options ===

	if (cmdline_parser(argc, argv, &config) != 0)
	{
		exit(1);
	}
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
	bool printSlash = false;

	if (config.inputs_num > 0)
	{
		pathDir = config.inputs[0];

		size_t size = strlen(pathDir);
		if (pathDir[size - 1] != '/')
		{
			printSlash = true;
		}
	}
	//=== loop through the directory stream ===
	if (config.sort_flag == true)
	{
		scandir(pathDir, printSlash);
	}
	else
	{
		readdir(pDir, pathDir, printSlash);
	}
	// close the directory stream
	closedir(pDir);

	return 0;
}

int filterDirEntry(const struct dirent* pEntry)
{
	if (strcmp(pEntry->d_name, ".") == 0 || strcmp(pEntry->d_name, "..") == 0)
	{
		return 0;
	}
	if (pEntry->d_type == DT_DIR && all_flag == false && config.dirs_flag == false)
	{
		return 0;
	}
	if (pEntry->d_type == DT_REG && all_flag == false && config.files_flag == false)
	{
		return 0;
	}
	if (pEntry->d_type == DT_LNK && all_flag == false && config.links_flag == false)
	{
		return 0;
	}
	return true;
}

void scandir(char* pathDir, bool printSlash)
{
	struct dirent** colEntry;
	int count;

	count = scandir(pathDir, &colEntry, &filterDirEntry, alphasort);
	if (count < 0)
	{ // returns -1 if an error occurs
		perror("scandir error");
		return;
	}
	for (int index = 0; index < count; index++)
	{
		struct dirent* pEntry;

		pEntry = colEntry[index];
		print(pEntry, pathDir, printSlash);

		free(pEntry);
	}
	free(colEntry);
}

void readdir(DIR* pDir, char* pathDir, bool printSlash)
{
	struct dirent* pEntry;

	while ((pEntry = readdir(pDir)) != NULL)
	{
		if (filterDirEntry(pEntry) > 0)
		{
			print(pEntry, pathDir, printSlash);
		}
	}
}

void print(struct dirent* pEntry, char * pathDir, bool printSlash)
{
	printf("%s", pathDir);
	if (printSlash == true)
	{
		printf("/");
	}
	printf("%s\n", pEntry->d_name);
}
