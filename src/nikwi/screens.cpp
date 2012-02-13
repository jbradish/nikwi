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

#include "nikwi.h"

Screen	*activeScreen = NULL;

//Screen
Screen::Screen()
{
}

Screen::~Screen()
{
}

void Screen::enter()
{
}

void Screen::leave()
{
}

void Screen::timer()
{
}

void Screen::draw()
{
}

void Screen::handleEvent(SDL_Event &e)
{
}

void Screen::drawCursor()
{
	if (cursor)
	{
		int		mx, my;
		SDL_GetMouseState(&mx, &my);
		cursor->frame[0]->draw(mx, my, false); 
	}
}

void Screen::activate()
{
	activateScreen(this);
}

//Globals
void initScreens()
{
}

void shutdownScreens()
{
}

void activateScreen(Screen *screen)
{
	if (activeScreen)
		activeScreen->leave();
	activeScreen = screen;
	screen->enter();
}

