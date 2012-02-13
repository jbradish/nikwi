/*
 *  Nikwi Deluxe
 *  Copyright (C) 2006  Kostas Michalopoulos
 *  
 *  A game by Kostas "Bad Sector" Michalopoulos
 *  email: badsector@slashstone.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
** Nikwi Engine - Menu screen
*/

#ifndef __MENU_H_INCLUDED__
#define __MENU_H_INCLUDED__

struct MenuScreen : public Screen
{
	int		opt;
	SDL_Surface	*background;
	SDL_Surface	*paused;
	SDL_Surface	*pwdBack;
	Sprite		*dot;
	char		pwd[7];
	int		pwdLen;
	bool		pwdMode;
	uint		pausedShow;
	Sprite		*sprite;
	SpriteState	*spriteState;
	float		a;
	int		y, ty;
	
	MenuScreen();
	virtual ~MenuScreen();
	
	virtual void enter();
	
	virtual void timer();
	
	virtual void draw();
	virtual void handleEvent(SDL_Event &e);
};

extern MenuScreen	*menuScreen;

#endif

