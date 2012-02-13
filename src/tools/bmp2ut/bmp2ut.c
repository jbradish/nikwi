/*
 * Nikwi Deluxe - bmp2ut
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
** bmp to Undead Texture converter
** written by Kostas "Bad Sector" Michalopoulos
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>

#ifdef main
#undef main
#endif

typedef unsigned int     uint;

char		*input = NULL;
char		*output = NULL;
char		*maskfn = NULL;
FILE		*f = NULL;
SDL_Surface	*surf = NULL;
SDL_Surface	*mask = NULL;
int		alpha = 0;
int		halfBits = 0;

void convert(void)
{
	char		id[3] = "UT";
	unsigned char	*rgb;
	unsigned char	*mrgb;
	unsigned char	flags = 0;
	uint		i;
	unsigned short	wv, hv;

	surf = SDL_LoadBMP(input);
	if (surf->format->BitsPerPixel != 24)
	{
		fprintf(stderr, "Only 24-bit pixels are supported!\n");
		SDL_Quit();
		exit(1);
	}
	if (halfBits)
		flags = 1;
	f = fopen(output, "wb");
	fwrite(id, 3, 1, f);
	fwrite(&flags, 1, 1, f);
	wv = surf->w;
	hv = surf->h;
	fwrite(&wv, 2, 1, f);
	fwrite(&hv, 2, 1, f);
	rgb = (unsigned char*)surf->pixels;
	mrgb = mask?((unsigned char*)mask->pixels):0;
	for (i=0;i<surf->w*surf->h;i++)
	{
		char		a = 255;
		unsigned char	b = *(rgb++);
		unsigned char	g = *(rgb++);
		unsigned char	r = *(rgb++);
		if (alpha)
			a = r;
		if (mask)
		{
			a = *(mrgb);
			mrgb += 3;
		}
		if (halfBits)
		{
			unsigned short	rgb;
			rgb = (b >> 3)|((g >> 2) << 5)|((r >> 3) << 11);
			fwrite(&rgb, 2, 1, f);
		}
		else
		{
			fwrite(&r, 1, 1, f);
			fwrite(&g, 1, 1, f);
			fwrite(&b, 1, 1, f);
			fwrite(&a, 1, 1, f);
		}
	}
	fclose(f);
	printf("wrote %ix%i image", surf->w, surf->h);
	if (alpha && !mask)
		printf(" using A=R");
	if (mask)
		printf(" using mask");
	printf("\n");
}

int main(int argn, char **argv)
{
	int	i;

	SDL_Init(0);

	for (i=0;i<argn;i++)
	{
		if (!strcmp(argv[i], "-?") || !strcmp(argv[i], "--help"))
		{
			printf("bmp to Undead Texture converter\n");
			printf("Usage: bmp2ut [options] -i <file.bmp> -o "
				"<file.ut>\n");
			printf("Options:\n");
			printf("-i --input	<file>	add infile\n");
			printf("-o --output	<file>	output file\n");
			printf("-a --alpha              Achannel = Rchannel\n");
			printf("-? --help		this screen\n");
			printf("--halfbits		use half bits to save\n"
							"disk space\n");
			printf("\n");
			printf("If <file> contains _a_ then the effect of "
			"the -a option is used.\n");
			printf("\n");
			return 0;
		}
		else
		if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input"))
		{
			input = argv[++i];
			if (strstr(input, "_mask."))
				return 0;
			if (strstr(input, "_a_"))
				alpha = 1;
		}
		else
		if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
		{
			output = argv[++i];
		}
		else
		if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--alpha"))
		{
			alpha = 1;
		}
		else
		if (!strcmp(argv[i], "--halfbits"))
		{
			halfBits = 1;
		}
		else
		if (argv[i][0] == '-')
		{
			printf("Unknown option: %s\n", argv[i]);
			return 1;
		}
	}

	if (!input)
	{
		printf("Input file not given: use --help for help\n");
		SDL_Quit();
		return 1;
	}
	
	if (!output)
	{
		int	i;
		output = malloc(1024);
		strcpy(output, input);
		for (i=strlen(output)-1;i != -1;i--)
			if (output[i] == '.')
			{
				output[i+1] = 'u';
				output[i+2] = 't';
				output[i+3] = 0;
				break;
			}
	}
	
	if (!maskfn)
	{
		int	i;
		FILE	*f;
		maskfn = malloc(1024);
		strcpy(maskfn, input);
		for (i=0;maskfn[i];i++)
			if (maskfn[i] == '.')
			{
				char	*tmp = malloc(1024);
				char	*tmp2 = malloc(1024);
				strcpy(tmp2, maskfn);
				tmp2[i] = 0;
				sprintf(tmp, "%s_mask%s", tmp2, maskfn + i);
				free(tmp2);
				free(maskfn);
				maskfn = tmp;
				break;
			}
		f = fopen(maskfn, "r");
		if (f)
		{
			fclose(f);
			mask = SDL_LoadBMP(maskfn);
		}
	}

	convert();

	if (surf)
		SDL_FreeSurface(surf);
	if (mask)
		SDL_FreeSurface(mask);

	SDL_Quit();

	return 0;
}












