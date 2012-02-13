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
** Nikwi Engine - Ending screen
*/

#include "nikwi.h"

EndingScreen	*ending = NULL;

EndingScreen::EndingScreen()
{
}

EndingScreen::~EndingScreen()
{
}

void EndingScreen::loadImages()
{
	story1 = loadImage("data/objects/story1.ut");
	story2 = loadImage("data/objects/story2.ut");
	story3 = loadImage("data/objects/story3.ut");
	story4 = loadImage("data/objects/story4.ut");
}

void EndingScreen::enter()
{
	Transition	*trans = new MosaicTransition();
	trans->begin();
	game->draw();
	trans->play();
	trans->begin();
	game->draw();
	trans->play();
	delete trans;
	trans = new LinesTransition();
	trans->begin();
	draw();
	trans->play();
	delete trans;
	tick = 0;
	loadImages();
}

void EndingScreen::leave()
{
	SDL_FreeSurface(story4);
	SDL_FreeSurface(story3);
	SDL_FreeSurface(story2);
	SDL_FreeSurface(story1);
}

void EndingScreen::timer()
{
	tick++;
	if (tick == 100 || tick == 300 || tick == 450 || tick == 750 ||
		tick == 1000)
	{
		Transition	*trans;
		if (tick == 100)
			trans = new MosaicTransition();
		else
			trans = new LinesTransition();
		trans->begin();
		draw();
		trans->play();
		delete trans;
	}
	if (tick == 1200)
	{
		imageScreen->nextImageFile = "data/objects/credits.ut";
		imageScreen->showImage(menuScreen, "data/objects/gover.ut",
			320);
	}
}

void EndingScreen::draw()
{
	SDL_Rect	r;
	
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	memset(screen->pixels, 0, screen->pitch*screen->h);
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	
	if (tick >= 100 && tick < 750)
	{
		r.x = 64;
		r.y = 16;	//160//
		if (SDL_BlitSurface(story1, NULL, screen, &r) != 0)
		{
			loadImages();
			SDL_Delay(100);
			return;
		}
	}
	if (tick >= 300 && tick < 750)
	{
		r.x = 64;
		r.y = 16+160;
		if (SDL_BlitSurface(story2, NULL, screen, &r) != 0)
		{
			loadImages();
			SDL_Delay(100);
			return;
		}
	}
	if (tick >= 450 && tick < 750)
	{
		r.x = 64;
		r.y = 16+320;
		if (SDL_BlitSurface(story3, NULL, screen, &r) != 0)
		{
			loadImages();
			SDL_Delay(100);
			return;
		}
	}
	if (tick >= 750 && tick < 1000)
	{
		r.x = 64;
		r.y = 176;
		if (SDL_BlitSurface(story4, NULL, screen, &r) != 0)
		{
			loadImages();
			SDL_Delay(100);
			return;
		}
	}
}

void EndingScreen::handleEvent(SDL_Event &e)
{
}

