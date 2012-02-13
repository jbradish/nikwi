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
** Nikwi Engine - World editor
*/

#include "nikwi.h"

Editor	*editor = NULL;

//Editor
Editor::Editor()
{
	aWorld = aTile = aObject = 0;
	tileMode = true;
	mirror = false;
	finePos = false;
}

Editor::~Editor()
{
}

void Editor::loadWorld()
{
	char	buff[32];
	sprintf(buff, "data/worlds/world%i.w", aWorld);
	world->loadWorld(buff);
	for (Object *ob=world->firstObject;ob;ob = ob->next)
		ob->visible = true;
	sprintf(buff, "Editing world #%i", aWorld);
	SDL_WM_SetCaption(buff, buff);
}

void Editor::enter()
{
	loadWorld();
}

void Editor::leave()
{
	char	buff[32];
	sprintf(buff, "data/worlds/world%i.w", aWorld);
	world->saveWorld(buff);
}

void Editor::timer()
{
}

void Editor::draw()
{
	world->draw();
	
	if (tileMode)
	{
		tileClass[aTile]->sprite->frame[0]->draw(mx, my, false);
	}
	else
	{
		objectClass[aObject]->sprite->frame[0]->draw(mx, my, mirror);
	}
	
	drawCursor();
}

void Editor::handleEvent(SDL_Event &e)
{
	switch (e.type)
	{
		case SDL_MOUSEBUTTONDOWN:
			switch (e.button.button)
			{
				case 1: // leftclick
					if (tileMode)
					{
						uint	ti;
						ti = (my/32)*world->width+mx/32;
						if (ti < 0)
							return;
						if (world->tile[ti])
							delete world->tile[ti];
						world->tile[ti] = new Tile(
							tileClass[aTile]->code);
						world->tile[ti]->x = mx;
						world->tile[ti]->y = my;
						world->refreshStaticPart = true;
					}
					else
					{
						Object	*ob;
						ob = world->createObject(
							objectClass[aObject]->
							code, mx, my);
						ob->state->mirror = mirror;
						world->recalcBobbing();
					}
					break;
				case 2: // middleclick
					tileMode = !tileMode;
					break;
				case 3: // rightclick
					if (tileMode)
					{
						uint	ti;
						ti = (my/32)*world->width+mx/32;
						if (ti < 0)
							return;
						if (world->tile[ti])
						{
							delete world->tile[ti];
							world->tile[ti] = NULL;
						}
						world->refreshStaticPart = true;
					}
					else
					{
						Object	*ob = world->objectAt(
							mx, my);
						if (ob)
							world->removeObject(ob);
					}
					break;
				case 4:	// wheel up
					if (tileMode)
					{
						if (aTile)
							aTile--;
						else
							aTile = tileClasses - 1;
					}
					else
					{
						if (aObject)
							aObject--;
						else
							aObject =
								objectClasses-1;
					}
					break;
				case 5:	// wheel down
					if (tileMode)
					{
						if (aTile < tileClasses - 1)
							aTile++;
						else
							aTile = 0;
					}
					else
					{
						if (aObject < objectClasses - 1)
							aObject++;
						else
							aObject = 0;
					}
					break;
			}
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
				case SDLK_q:
					world->bgndImage = 0;
					world->refreshStaticPart = true;
					break;
				case SDLK_w:
					world->bgndImage = 1;
					world->refreshStaticPart = true;
					break;
				case SDLK_e:
					world->bgndImage = 2;
					world->refreshStaticPart = true;
					break;
				case SDLK_r:
					world->bgndImage = 3;
					world->refreshStaticPart = true;
					break;
				case SDLK_t:
					world->bgndImage = 4;
					world->refreshStaticPart = true;
					break;
				case SDLK_y:
					world->bgndImage = 5;
					world->refreshStaticPart = true;
					break;
				case SDLK_u:
					world->bgndImage = 6;
					world->refreshStaticPart = true;
					break;
				case SDLK_i:
					world->bgndImage = 7;
					world->refreshStaticPart = true;
					break;
				case SDLK_o:
					world->bgndImage = 8;
					world->refreshStaticPart = true;
					break;
				case SDLK_p:
					world->bgndImage = 9;
					world->refreshStaticPart = true;
					break;
				case SDLK_a:
					world->bgndImage = 10;
					world->refreshStaticPart = true;
					break;
				case SDLK_s:
					world->bgndImage = 11;
					world->refreshStaticPart = true;
					break;
				case SDLK_1:
					world->bgnd = 0;
					world->refreshStaticPart = true;
					break;
				case SDLK_2:
					world->bgnd = 1;
					world->refreshStaticPart = true;
					break;
				case SDLK_3:
					world->bgnd = 2;
					world->refreshStaticPart = true;
					break;
				case SDLK_4:
					world->bgnd = 3;
					world->refreshStaticPart = true;
					break;
				case SDLK_5:
					world->bgnd = 4;
					world->refreshStaticPart = true;
					break;
				case SDLK_F3:
					enter();
					break;
				case SDLK_F2:
					leave();
					break;
				case SDLK_F6:
					game->level = aWorld;
					game->restartLevel();
					game->activate();
					break;
				case SDLK_F8:
				{
					static bool	firstF8 = true;
					if (firstF8)
						firstF8 = false;
					else
					{
						firstF8 = true;
						world->newWorld(20, 15);
					}
					break;
				}
				case SDLK_PAGEDOWN:
					leave();
					aWorld++;
					enter();
					break;
				case SDLK_PAGEUP:
					leave();
					if (aWorld)
						aWorld--;
					enter();
					break;
				case SDLK_TAB:
					finePos = !finePos;
					break;
				case SDLK_SPACE:
					mirror = !mirror;
					break;
				default:;
			}
			break;
		case SDL_MOUSEMOTION:
			if (tileMode || finePos)
			{
				mx = e.motion.x/32*32;
				my = e.motion.y/32*32;
			}
			else
			{
				mx = e.motion.x/2*2;
				my = e.motion.y/2*2;
			}
			break;
	}
}

