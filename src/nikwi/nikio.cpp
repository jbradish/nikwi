/*
 * Nikwi Deluxe
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

/*
** Nikwi Engine - I/O (mostly I, less O)
*/

#include "nikwi.h"

struct PackEntry
{
	String	name;
	uint	start;
	uint	size;
};

static bool		usePackFile = false;
static String		packFileName;
static uint		dirPlace, entryCount;
static PackEntry	*entry;

void initNikIO()
{
	FILE	*f;
	packFileName = "justdata.up";
	f = fopen("justdata.up", "rb");
	if (!f)
	{
		packFileName = "justdata.sw";
		f = fopen("justdata.sw", "rb");
	}
	if (f)
	{
		fseek(f, 3, SEEK_SET);
		fread(&dirPlace, 4, 1, f);
		fread(&entryCount, 4, 1, f);
		fseek(f, dirPlace, SEEK_SET);
		entry = (PackEntry*)malloc(sizeof(PackEntry)*entryCount);
		for (uint i=0;i<entryCount;i++)
		{
			unsigned short	len;
			uint		start, size;
			String		name;
			fread(&len, 2, 1, f);
			name = (String)malloc(len + 1);
			fread(name, len, 1, f);
			name[len] = 0;
			fread(&start, 4, 1, f);
			fread(&size, 4, 1, f);
			entry[i].name = name;
			entry[i].start = start;
			entry[i].size = size;
		}
		fclose(f);
		usePackFile = true;
	}
	else
		usePackFile = false;
}

void shutdownNikIO()
{
	for (uint i=0;i<entryCount;i++)
		free(entry[i].name);
	free(entry);
}

static void *getDataFromPackFile(String file, uint &length)
{
	uint	start = 0;
	for (uint i=0;i<entryCount;i++)
		if (!strcmp(file, entry[i].name))
		{
			start = entry[i].start;
			length = entry[i].size;
			break;
		}
	
	if (!start)
	{
		//fprintf(stderr, "Asked for `%s' but it wasn't found\n");
		return NULL;
	}
		
	FILE	*f = fopen(packFileName, "rb");
	void	*data = malloc(length);
	fseek(f, start, SEEK_SET);
	fread(data, length, 1, f);
	fclose(f);
	
	return data;
}

static void *getDataFromFile(String file, uint &length)
{
	FILE	*f = fopen(file, "rb");
	size_t	size;
	void	*buff;
	if (!f)
		return 0;
	
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	buff = (void*)malloc(size);
	fread(buff, 1, size, f);
	
	fclose(f);
	
	length = size;
	return buff;
}

void *getData(String file, uint &length)
{
	if (usePackFile)
		return getDataFromPackFile(file, length);
	else
		return getDataFromFile(file, length);
}

String getStringData(String file)
{
	uint	len;
	String	buff = (String)getData(file, len);
	buff = (String)realloc(buff, len + 1);
	buff[len] = 0;
	return buff;
}

