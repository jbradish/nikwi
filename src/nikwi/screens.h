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
** Nikwi Engine - Screens
*/

#ifndef __SCREENS_H_INCLUDED__
#define __SCREENS_H_INCLUDED__

struct Screen
{
	Screen();
	virtual ~Screen();
	
	virtual void enter();
	virtual void leave();
	
	virtual void timer();
	
	virtual void draw();
	virtual void handleEvent(SDL_Event &e);
	
	void drawCursor();
	void activate();
};

extern Screen	*activeScreen;

void initScreens();
void shutdownScreens();

void activateScreen(Screen *screen);

#endif

