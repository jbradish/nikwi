/*
 * Nikwi Deluxe - upack
 * Copyright (C) 2005-2012 Kostas Michalopoulos
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Kostas Michalopoulos <badsector@runtimelegend.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#ifdef __linux__
#include <unistd.h>
#endif
#ifdef WIN32
#include <io.h>
#endif

typedef struct DirEntry
{
	char		*name;
	unsigned int	pos;
	unsigned int	size;
	struct DirEntry	*next;
} DirEntry;

DirEntry	*entries = NULL;
DirEntry	*lastEntry = NULL;
unsigned int	entryCount = 0;
unsigned int	priority = 1000;
char		*input = "data";
char		*output = "justdata.up";
FILE		*f;

static void addEntry(char *name, unsigned int pos, unsigned int size)
{
	DirEntry	*de = malloc(sizeof(DirEntry));
	de->name = strdup(name);
	de->pos = pos;
	de->size = size;
	de->next = NULL;
	if (lastEntry)
		lastEntry->next = de;
	else
		entries = de;
	lastEntry = de;
	entryCount++;
}

static void freeEntries(void)
{
	while (entries)
	{
		lastEntry = entries->next;
		free(entries->name);
		free(entries);
		entries = lastEntry;
	}
}

static void startPackage(void)
{
	char		id[2] = "UP";
	char		version = 1;
	unsigned int	dirPlace = 0;
	f = fopen(output, "wb");
	if (!f)
	{
		printf("Cannot create `%s'\n", output);
		exit(1);
	}
	fwrite(id, 2, 1, f);
	fwrite(&version, 1, 1, f);
	fwrite(&dirPlace, 4, 1, f);
	fwrite(&entryCount, 4, 1, f);
	fwrite(&priority, 4, 1, f);
}

static void saveDirectoryEntries()
{
	unsigned int	dirPlace = ftell(f);
	DirEntry	*de = entries;
	printf("Writing directory: ");
	for (;de;de = de->next)
	{
		unsigned short	len = strlen(de->name);
		fwrite(&len, 2, 1, f);
		fwrite(de->name, len, 1, f);
		fwrite(&de->pos, 4, 1, f);
		fwrite(&de->size, 4, 1, f);
	}
	fseek(f, 3, SEEK_SET);
	fwrite(&dirPlace, 4, 1, f);
	fwrite(&entryCount, 4, 1, f);
	printf("%i entries written\n", entryCount);
}

static void packFile(char *filename, char *entryname)
{
	FILE		*inf = fopen(filename, "rb");
	char		buff[16384];
	int		rb, wb;
	unsigned int	pos = ftell(f);
	unsigned int	size;
	if (!inf)
	{
		printf("failed!\n");
		return;
	}
	fseek(inf, 0, SEEK_END);
	size = ftell(inf);
	fseek(inf, 0, SEEK_SET);
	while (!feof(inf))
	{
		rb = fread(buff, 1, sizeof(buff), inf);
		wb = fwrite(buff, 1, rb, f);
		if (rb != wb)
		{
			printf("disk full!\n");
			fclose(inf);
			return;
		}
	}
	fclose(inf);
	
	addEntry(entryname, pos, size);
	printf("ok size=%i kb\n", size/1024 + 1);
}

static void scanDirectory(char *dirname, char *path)
{
	DIR		*dir;
	char		*spath;
	struct dirent	*de;
	if (chdir(dirname))
		return;
	
	spath = malloc(strlen(path) + strlen(dirname) + 2);
	if (path[0])
		sprintf(spath, "%s/%s", path, dirname);
	else
		sprintf(spath, "%s", dirname);
	dir = opendir(".");
	if (!dir)
	{
		chdir("..");
		printf("cannot open `%s'\n", dirname);
		return;
	}
	while ((de = readdir(dir)))
	{
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		if (chdir(de->d_name))
		{
			char	buff[2048];
			sprintf(buff, "%s/%s", spath, de->d_name);
			printf("Packing `%s': ", buff);
			packFile(de->d_name, buff);
		}
		else
		{
			chdir("..");
			if (de->d_name[0] == '_')
			{
				printf("Skipping directory `%s/%s'\n", spath,
					de->d_name);
				continue;
			}
			scanDirectory(de->d_name, spath);
		}
	}
	closedir(dir);
		
	free(spath);
	chdir("..");
}

int main(int argn, char **argv)
{
	int	i;
	for (i=0;i<argn;i++)
	{
		if (!strcmp(argv[i], "--help"))
		{
			printf("Undead Packer version 1.0.\n");
			printf("Usage: upack [-o filename.up] [-p priority] ");
			printf("[-d data_directory]\n");
			printf("-o   sets the output (default is justdata.up)");
			printf("\n-p   sets the package priority. More means "
			"higher priority. Core packages\nshould be in the range"
			" 0-99 and expansions in the range 100-999. User "
			"packages\nshould use priority 1000. Default is 1000.");
			printf("\n-d   defines the input data directory. "
			"The contents of this directory will be\npacked into "
			"the packfile and the name of the directory will be "
			"prefixed to\nall file names.\n\n");
			return 0;
		}
		else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
		{
			output = argv[++i];
		}
		else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--output"))
		{
			priority = atoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--data"))
		{
			input = argv[++i];
		}
	}
	
	startPackage();
	
	scanDirectory(input, "");
	
	saveDirectoryEntries();
	
	fseek(f, 0, SEEK_END);
	printf("Finished. Packed `%s' in `%s' with priority=%i, length=%ukb.\n",
		input, output, priority, (unsigned int)(ftell(f)/1024 + 1));
	if (priority < 100)
		printf("Warning: used 'Core data' priority!\n");
	else if (priority < 1000)
		printf("Warning: used 'Expansion data' priority!\n");
	else if (priority > 1000)
		printf("Warning: used lower priority than 'user data'\n");
	fclose(f);
	
	freeEntries();
	
	return 0;
}
