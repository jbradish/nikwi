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
** Nikwi Engine - World
*/

#ifndef __WORLD_H_INCLUDED__
#define __WORLD_H_INCLUDED__

struct World
{
	Tile		**tile;
	uint		width;
	uint		height;
	bool		refreshStaticPart;
	SDL_Surface	*staticPart;
	Tile		**nonStaticTile;
	uint		nonStaticTiles;
	Object		*firstObject;
	Object		*lastObject;
	uint		bgnd;
	uint		bgndImage;
	Object		*hero;
	Object		*icecream;
	uint		candyCount;	
	
	SlashTDP::World	world;
	
	World();
	~World();
	
	void newWorld(int nwidth, int nheight);
	void loadWorld(String file);
	void saveWorld(String file);
	
	void prepareForPhysics();
	
	Object *createObject(uint code, int x, int y);
	Object *createParticleObject(uint code, int x, int y, float a, float f,
		int life);
	void removeObject(Object *obj);
	void removeAllObjects();
	void recalcBobbing();
	
	Object *objectAt(int x, int y);
	
	Tile *objectCollideWithWorld(Object *obj);

	void drawStaticPart();	
	void draw();
	
	void animate();
};

extern World	*world;

#endif

