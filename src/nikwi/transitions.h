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

#ifndef __TRANSITIONS_H_INCLUDED__
#define __TRANSITIONS_H_INCLUDED__

struct Transition
{
	unsigned short	*screen1;
	unsigned short	*screen2;
	int		pitch1;
	int		pitch2;
	SDL_Surface	*tempScreen;
	SDL_Surface	*saveScreen;
	
	Transition();
	virtual ~Transition();
	
	void grab(unsigned short *target);
	void begin();
	void end();
	void updateScreen();
	
	void beginPlay();

	virtual void play()=0;
};

struct RevealTransition : public Transition
{
	RevealTransition();
	virtual ~RevealTransition();
	
	virtual void play();
};

struct LinesTransition : public Transition
{
	LinesTransition();
	virtual ~LinesTransition();
	
	virtual void play();
};

struct MosaicTransition : public Transition
{
	MosaicTransition();
	virtual ~MosaicTransition();
	
	void bar(int x1, int y1, int x2, int y2, int color);
	void applyMosaic(int size, unsigned short *from, int pitch);
	
	virtual void play();
};

struct DoubleLinesTransition : public Transition
{
	DoubleLinesTransition();
	virtual ~DoubleLinesTransition();
	
	virtual void play();
};

Transition *createRandomTransition();

#endif

