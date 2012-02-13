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
** Nikwi Engine - Menu screen
*/

#include "nikwi.h"

MenuScreen	*menuScreen = NULL;

MenuScreen::MenuScreen()
{
	background = loadImage("data/objects/title.ut");
	paused = loadImage("data/objects/paused.ut");
	pwdBack = loadImage("data/objects/pwd2.ut");
	pwdMode = false;
	dot = findSprite("dot");
	sprite = NULL;
	spriteState = NULL;
	opt = 0;
	a = 0;
	y = ty = 240;
}

MenuScreen::~MenuScreen()
{
	if (spriteState)
		delete spriteState;
	SDL_FreeSurface(pwdBack);
	SDL_FreeSurface(paused);
	SDL_FreeSurface(background);
}

void MenuScreen::enter()
{
	DoubleLinesTransition	*trans = new DoubleLinesTransition;
	trans->begin();
	draw();
	trans->play();
	delete trans;
}

void MenuScreen::timer()
{
	if (y != ty)
	{
		if (y > ty)
		{
			a = -3.14f;
			y -= 6;
			if (y < ty)
				y = ty;
		}
		else
		{
			a = 0;
			y += 6;
			if (y > ty)
				y = ty;
		}
	}
	else
		a += 0.3f;
	pausedShow++;
}

void MenuScreen::draw()
{
	if (SDL_BlitSurface(background, NULL, screen, NULL) != 0)
	{
		SDL_FreeSurface(background);
		background = loadImage("data/objects/title.ut");
		SDL_Delay(100);
		draw();
		return;
	}
	if (game->playing)
	{
		SDL_Rect	r;
		if (((pausedShow/20) % 4))
		{
			r.x = 160;
			r.y = 397;
			if (SDL_BlitSurface(paused, NULL, screen, &r) != 0)
			{
				SDL_FreeSurface(paused);
				paused = loadImage("data/objects/paused.ut");
				SDL_Delay(100);
				draw();
				return;
			}
		}
	}
	switch (opt)
	{
		case 0:
			ty = 240;
			break;
		case 1:
			ty = 274;
			break;
		case 2:
			ty = 308;
			break;
		case 3:
			ty = 342;
			break;
	}
	if (!sprite)
	{
		sprite = findSprite("candy1");
		spriteState = new SpriteState(sprite);
	}
	spriteState->mirror = false;
	spriteState->draw(200, y-3+sin(a)*2);
	spriteState->mirror = true;
	spriteState->draw(411, y-3+sin(a)*2);
	if (pwdMode)
	{
		SDL_Rect	r;
		int		x = 224;
		r.x = 129;
		r.y = 200;
		if (SDL_BlitSurface(pwdBack, NULL, screen, &r) != 0)
		{
			SDL_FreeSurface(pwdBack);
			pwdBack = loadImage("data/objects/pwd2.ut");
			SDL_Delay(100);
			draw();
			return;
		}
		for (int i=0;i<pwdLen;i++,x+=32)
			dot->frame[0]->draw(x, 270, false);
	}
}

void MenuScreen::handleEvent(SDL_Event &e)
{
	if (e.type != SDL_KEYDOWN)
		return;
		
	if (pwdMode)
	{
		if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z)
		{
			if (pwdLen == 6)
			{
				playSample(findSample(makeCode("slct")));
				return;
			}
			playSample(findSample(makeCode("tick")));
			pwd[pwdLen++] = 'a'+(e.key.keysym.sym-SDLK_a);
			return;
		}
		if (e.key.keysym.sym == SDLK_BACKSPACE)
		{
			if (!pwdLen)
			{
				playSample(findSample(makeCode("slct")));
				return;
			}
			playSample(findSample(makeCode("tick")));
			pwdLen--;
			return;
		}
		if (e.key.keysym.sym == SDLK_RETURN)
		{
			playSample(findSample(makeCode("tick")));
			if (pwdLen == 6)
			{
				pwd[6] = 0;
				game->newGame(pwd);
				e.key.keysym.sym = SDLK_a;
				game->activate();
			}
			pwdMode = false;
			return;
		}
		if (e.key.keysym.sym == SDLK_ESCAPE)
			pwdMode = false;
		return;
	}
	
	switch (e.key.keysym.sym)
	{
		case SDLK_DOWN:
			if (opt == 3)
				opt = 0;
			else
				opt++;
			playSample(findSample(makeCode("slct")));
			break;
		case SDLK_UP:
			if (opt)
				opt--;
			else
				opt = 3;
			playSample(findSample(makeCode("slct")));
			break;
		case SDLK_SPACE:
		case SDLK_RETURN:
		{
			playSample(findSample(makeCode("tick")));
			switch (opt)
			{
				case 0:
					game->newGame(); 
					game->activate();
					break;
				case 1:
					pwdLen = 0;
					pwdMode = true;
					break;
				case 2:
					imageScreen->showImage(this,
						"data/objects/credits.ut");
					break;
				case 3:
					running = false;
					break;
			}
			break;
		}
		case SDLK_ESCAPE:
			if (game->playing)
				game->activate();
			break;
		default:;
	}
}

