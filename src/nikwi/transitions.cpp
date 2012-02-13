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
** Nikwi Engine - Transitions
*/

#include "nikwi.h"

//Transition
Transition::Transition()
{
	screen1 = screen2 = NULL;
	tempScreen = saveScreen = NULL;
}

Transition::~Transition()
{
	if (tempScreen)
		SDL_FreeSurface(tempScreen);
	if (screen1)
		free(screen1);
	if (screen2)
		free(screen2);
}

void Transition::grab(unsigned short *target)
{
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	memcpy(target, screen->pixels, screen->pitch*screen->h);
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
}

void Transition::begin()
{
	if (!screen1)
		screen1 = (unsigned short*)malloc(screen->pitch*screen->h);
	grab(screen1);
	pitch1 = screen->pitch/2;
	if (!tempScreen)
		tempScreen = createSurface(screen->w, screen->h, false);
	saveScreen = screen;
	screen = tempScreen;
	if (!screen2)
		screen2 = (unsigned short*)malloc(screen->pitch*screen->h);
}

void Transition::end()
{
	if (screen1)
		free(screen1);
	if (screen2)
		free(screen2);
	screen1 = screen2 = NULL;
}

void Transition::updateScreen()
{
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	memcpy(screen->pixels, screen1, screen->h*screen->pitch);
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	updateSystemScreen();
}

void Transition::beginPlay()
{
	grab(screen2);
	pitch2 = screen->pitch/2;
	screen = saveScreen;
	if (tempScreen)
		SDL_FreeSurface(tempScreen);
	tempScreen = NULL;
}


//RevealTransition
RevealTransition::RevealTransition()
{
}

RevealTransition::~RevealTransition()
{
}

void RevealTransition::play()
{
	beginPlay();
	
	for (int y=0;y<480;y+=10)
	{
		memcpy(&screen1[y*pitch1], &screen2[y*pitch2], 12800);
		updateScreen();
		SDL_Delay(1);
	}
	
	end();
}


//LinesTransition
LinesTransition::LinesTransition()
{
}

LinesTransition::~LinesTransition()
{
}

void LinesTransition::play()
{
	beginPlay();
	
	for (int y=0;y<10;y++)
	{
		for (int yy=0;yy<48;yy++)
			memcpy(&screen1[(yy*10+y)*pitch1],
				&screen2[(yy*10+y)*pitch2], 1280);
		updateScreen();
		SDL_Delay(10);
	}
	
	end();
}


//MosaicTransition
MosaicTransition::MosaicTransition()
{
}

MosaicTransition::~MosaicTransition()
{
}

void MosaicTransition::bar(int x1, int y1, int x2, int y2, int color)
{
	unsigned short	*dst = &screen1[y1*pitch1 + x1];
	unsigned short	line[1280];
	int		w = (x2 - x1 + 1)*2;
	for (int x=0;x<w/2;x++)
		line[x] = color;
	for (int y=y1;y<=y2;y++,dst+=pitch1)
		memcpy(dst, line, w);
}

void MosaicTransition::applyMosaic(int size, unsigned short *from, int pitch)
{
	for (int y=0;y<screen->h;y += size)
		for (int x=0;x<screen->w;x += size)
		{
			int	sample = from[(y/size*size+size/2)*pitch + 
				(x/size*size+size/2)];
			bar(x, y, x+size-1, y+size-1, sample);
		}
}

void MosaicTransition::play()
{
	beginPlay();

	for (int i=2;i<32;i=i*2)
	{
		applyMosaic(i, screen1, pitch1);	
		updateScreen();
		SDL_Delay(30);
	}
	for (int i=32;i>0;i=i/2)
	{
		applyMosaic(i, screen2, pitch2);	
		updateScreen();
		SDL_Delay(30);
	}
	
	end();
}


//DoubleLinesTransition
DoubleLinesTransition::DoubleLinesTransition()
{
}

DoubleLinesTransition::~DoubleLinesTransition()
{
}

void DoubleLinesTransition::play()
{
	beginPlay();
	
	for (int y=0;y<480;y+=2)
	{
		int	y2 = 479-y;
		memcpy(&screen1[y*pitch1], &screen2[y*pitch2], 1280);
		memcpy(&screen1[y2*pitch1], &screen2[y2*pitch2], 1280);

		if (!(y&0x0F))
		{
			updateScreen();
			SDL_Delay(10);
		}
	}
	updateScreen();
	
	end();
}


//Functions
Transition *createRandomTransition()
{
	int	r = rand()%32;
	if (r > 16)
		return new MosaicTransition();
	if (r < 4)
		return new RevealTransition();
	if (r < 9)
		return new LinesTransition();
	return new DoubleLinesTransition();
}
