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

#ifndef __IMAGESCREEN_H_INCLUDED__
#define __IMAGESCREEN_H_INCLUDED__

struct ImageScreen : public Screen
{
	SDL_Surface	*image;
	char		*imageFile;
	Screen		*backScreen;
	bool		timeOutEnabled;
	int		timeValue, timeOut;
	char		*nextImageFile;
	bool		exitAfter;
	
	ImageScreen();
	virtual ~ImageScreen();
	
	void showImage(Screen *from, char *file, int timeOutIn=-1);
	
	virtual void timer();
	
	virtual void draw();
	virtual void handleEvent(SDL_Event &e);
};

extern ImageScreen	*imageScreen;

#endif

