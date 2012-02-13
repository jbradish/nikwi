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
** Nikwi Engine - Image screen
*/

#include "nikwi.h"

ImageScreen	*imageScreen = NULL;

ImageScreen::ImageScreen()
{
	image = NULL;
	exitAfter = false;
	nextImageFile = NULL;
}

ImageScreen::~ImageScreen()
{
	if (image)
		SDL_FreeSurface(image);
}

void ImageScreen::showImage(Screen *from, char *file, int timeOutIn)
{
	backScreen = from;
	if (image)
		SDL_FreeSurface(image);
	image = loadImage(file);
	imageFile = file;
	
	activate();
	
	DoubleLinesTransition	*trans = new DoubleLinesTransition;
	trans->begin();
	draw();
	trans->play();
	delete trans;
	
	if (timeOutIn != -1)
	{
		timeOutEnabled = true;
		timeOut = timeOutIn;
		timeValue = 0;
	}
	else
		timeOutEnabled =false;

	if (exitAfter)
		SDL_Delay(2000);
	
	resetTimer();
}

void ImageScreen::timer()
{
	if (!timeOutEnabled)
		return;
	timeValue++;
	if (timeValue == timeOut)
	{
		SDL_FreeSurface(image);
		image = NULL;
		if (nextImageFile)
		{
			char	*save = nextImageFile;
			nextImageFile = NULL;
			showImage(backScreen, save, timeOut);
			return;
		}
		if (exitAfter)
			running = false;
		else
			backScreen->activate();
	}
}

void ImageScreen::draw()
{
	int	r;
	if ((r=SDL_BlitSurface(image, NULL, screen, NULL)) != 0)
	{
		if (r == -2)
		{
			SDL_FreeSurface(image);
			image = loadImage(imageFile);
		}
		SDL_Delay(100);
		draw();
		return;
	}
}

void ImageScreen::handleEvent(SDL_Event &e)
{
	if (e.type == SDL_KEYDOWN)
	{
		playSample(findSample(makeCode("tick")));
		SDL_FreeSurface(image);
		image = NULL;
		if (nextImageFile)
		{
			char	*save = nextImageFile;
			nextImageFile = NULL;
			showImage(backScreen, save, timeOut);
			return;
		}
		if (exitAfter)
			running = false;
		else
			backScreen->activate();
	}
}

