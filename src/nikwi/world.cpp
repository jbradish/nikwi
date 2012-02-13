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

#include "nikwi.h"

World	*world = NULL;

//World
World::World()
{
	width = 20;
	height = 15;
	bgnd = bgndImage = 0;
	staticPart = NULL;
	nonStaticTiles = 0;
	firstObject = lastObject = NULL;
	refreshStaticPart = false;
	
	tile = (Tile**)malloc(sizeof(Tile*)*width*height);
	memset(tile, 0, sizeof(Tile*)*width*height);
	
	for (int i=0;i<20;i++)
	{
		tile[i] = new Tile(makeCode("brk1"));
		tile[i]->x = i*32;
		tile[i]->y = 0;
		tile[14*20 + i] = new Tile(makeCode("brk1"));
		tile[14*20 + i]->x = i*32;
		tile[14*20 + i]->y = 14*32;
	}

	for (int i=1;i<14;i++)
	{
		tile[i*20] = new Tile(makeCode("brk2"));
		tile[i*20]->x = 0;
		tile[i*20]->y = i*32;
		tile[i*20 + 19] = new Tile(makeCode("brk5"));
		tile[i*20 + 19]->x = 19*32;
		tile[i*20 + 19]->y = i*32;
	}
}

World::~World()
{
	newWorld(0, 0);
}

void World::newWorld(int nwidth, int nheight)
{
	candyCount = 0;
	removeAllObjects();
	for (uint i=0;i<width*height;i++)
	{
		if (tile[i])
			delete tile[i];
		tile[i] = NULL;
	}
	free(tile);
	
	bgnd = 0;
	
	if (nwidth)
	{
		width = nwidth;
		height = nheight;
		tile = (Tile**)malloc(sizeof(Tile*)*width*height);
		memset(tile, 0, sizeof(Tile*)*width*height);
	}
	hero = NULL;
	refreshStaticPart = true;
	if (nwidth)
		prepareForPhysics();
}

void World::loadWorld(String file)
{
	uint	len, oc;
	char	*data = (char*)getData(file, len);
	uint	*tileCode, *iv;
	if (!data)
		return;
	newWorld(*((uint*)&data[0]), *((uint*)&data[4]));
	tileCode = (uint*)&data[8];
	for (uint y=0;y<height;y++)
	{
		for (uint x=0;x<width;x++,tileCode++)
		{
			if (*tileCode)
			{
				tile[y*width + x] = new Tile(*tileCode);
				tile[y*width + x]->x = x*32;
				tile[y*width + x]->y = y*32;
			}
		}
	}

	prepareForPhysics();

	oc = *tileCode;
	iv = ++tileCode;
	for (uint i=0;i<oc;i++)
	{
		uint	code = *(iv++);
		uint	flags = (*(iv++))&0x1;
		int	x = (int)*(iv++);
		int	y = (int)*(iv++);
		Object	*obj = createObject(code, x, y);
		obj->state->mirror = flags;
		if (code == makeCode("hero"))
			hero = obj;
		else if (code == makeCode("icrm"))
		{
			obj->visible = false;
			icecream = obj;
		}
		obj->init();
	}
	bgnd = *(iv++);
	bgndImage = *iv;
	free(data);
	
	refreshStaticPart = true;
	recalcBobbing();
	
	if (hero)
	{
		hero->createMotionController();
		//hero->createPhysicsController();
		//hero->body->gravity = 0.25f;
		hero->mc->gravity = 0.3f;
	}
}

void World::saveWorld(String file)
{
	FILE	*f = fopen(file, "wb");
	uint	oc = 0;
	fwrite(&width, 4, 1, f);
	fwrite(&height, 4, 1, f);
	for (uint i=0;i<width*height;i++)
	{
		if (tile[i])
			fwrite(&tile[i]->tclass->code, 4, 1, f);
		else
			fwrite(&tile[i], 4, 1, f);	//zero//
	}
	for (Object *obj=firstObject;obj;obj = obj->next)
		oc++;
	fwrite(&oc, 4, 1, f);
	for (Object *obj=firstObject;obj;obj = obj->next)
	{
		fwrite(&obj->oclass->code, 4, 1, f);
		fwrite(&obj->state->mirror, 4, 1, f);
		fwrite(&obj->x, 4, 1, f);
		fwrite(&obj->y, 4, 1, f);
	}
	fwrite(&bgnd, 4, 1, f);
	fwrite(&bgndImage, 4, 1, f);
	fclose(f);
}

void World::prepareForPhysics()
{
	world.newWorld();
	for (uint y=0;y<height;y++)
		for (uint x=0;x<width;x++)
			if (tile[y*width + x])
			{
				Tile	*t = tile[y*width + x];
				float	x1 = (x*32) + t->tclass->sprite->bx1;
				float	y1 = (y*32) + t->tclass->sprite->by1;
				float	x2 = (x*32) + t->tclass->sprite->bx2;
				float	y2 = (y*32) + t->tclass->sprite->by2;
				world.addBox(x1, y1, x2, y2);
			}
	world.optimize();
}

Object *World::createObject(uint code, int x, int y)
{
	Object	*obj = new Object(code);
	obj->prev = lastObject;
	if (lastObject)
		lastObject->next = obj;
	else
		firstObject = obj;
	lastObject = obj;
	
	obj->x = x;
	obj->y = y;
	
	return obj;
}

Object *World::createParticleObject(uint code, int x, int y, float a, float f,
	int life)
{
	Object	*obj = createObject(code, x, y);
	obj->createMotionController();
	obj->mc->gravity = 0.0f;
	obj->mc->mx = cos(a*3.14f/180.0f)*f;
	obj->mc->my = sin(a*3.14f/180.0f)*f;
	obj->life = life;
	obj->mc->solid = false;
	return obj;
}

void World::removeObject(Object *obj)
{
	if (obj->prev)
		obj->prev->next = obj->next;
	else
		firstObject = obj->next;
	if (obj->next)
		obj->next->prev = obj->prev;
	else
		lastObject = obj->prev;
	delete obj;
}

void World::recalcBobbing()
{
	float	bobPos = 0;
	for (Object *obj=firstObject;obj;obj=obj->next)
		if (obj->oclass->bobbing)
		{
			obj->bobPos = bobPos;
			bobPos += 1.0f;
		}
}

Object *World::objectAt(int x, int y)
{
	for (Object *obj=firstObject;obj;obj=obj->next)
		if (x >= obj->x && y >= obj->y && x < obj->x + obj->w &&
			y < obj->y + obj->h)
			return obj;
	return NULL;
}

Tile *World::objectCollideWithWorld(Object *obj)
{
	int	ox1 = obj->x, oy1 = obj->y, ox2 = ox1 + 31, oy2 = oy1 + 31;

	for (uint i=0;i<width*height;i++)
		if (tile[i] && rectOverRect(ox1, oy1, ox2, oy2, tile[i]->x,
			tile[i]->y, tile[i]->x + 31, tile[i]->y + 31))
				return tile[i];
	return NULL;
}

void World::removeAllObjects()
{
	while (firstObject)
	{
		lastObject = firstObject->next;
		delete firstObject;
		firstObject = lastObject;
	}
}

void World::drawStaticPart()
{
	float		r, g, b, rs, gs, bs;
	unsigned short	*dest;
	SDL_LockSurface(screen);
	
	switch (bgnd)
	{
		case 0:
			r = 168.0f;
			g = 212.0f;
			b = 235.0f;
			rs = 0.139583333f;
			gs = 0.047916667f;
			bs = 0;
			break;
		case 1:
			r = 83.0f;	//to 178,191,205
			g = 164.0f;
			b = 255.0f;
			rs = 0.197916667f;
			gs = 0.05625f;
			bs = -0.104166667f;
			break;
		case 2:
			r = 178.0f;	//to 149,120,167
			g = 205.0f;
			b = 193.0f;
			rs = -0.060416667f;
			gs = -0.177083333f;
			bs = -0.054166667;
			break;
		case 3:
			r = 141.0f;	//to 92,69,47
			g = 157.0f;
			b = 123.0f;
			rs = -0.102083333f;
			gs = -0.183333333f;
			bs = -0.158333333f;
			break;
		case 4:
			r = 197.0f;	//to 225,175,189
			g = 216.0f;
			b = 237.0f;
			rs = 0.0375;
			gs = -0.085416667f;
			bs = -0.1f;
			break;
	}
	
	Sprite		*spr = findSprite("candy1");
	
	switch (bgndImage)
	{
		case 1:
			spr = findSprite("candy2");
			break;
		case 2:
			spr = findSprite("candy3");
			break;
		case 3:
			spr = findSprite("watercup");
			break;
		case 4:
			spr = findSprite("icecream");
			break;
		case 5:
			spr = findSprite("chokolata");
			break;
		case 6:
			spr = findSprite("biscuit");
			break;
		case 7:
			spr = findSprite("lollipop");
			break;
		case 8:
			spr = findSprite("lollipop2");
			break;
		case 9:
			spr = findSprite("dcherry");
			break;
		case 10:
			spr = findSprite("icecreamcandy");
			break;
		case 11:
			spr = findSprite("icecreamcandy2");
			break;
		case 255:
			spr = NULL;
			break;
	}
	
	unsigned short	*sprp = spr?spr->frame[0]->localCopy:NULL;
	
	dest = (unsigned short*)screen->pixels;
	for (uint y=0;y<480;y++,r+=rs,g+=gs,b+=bs)
	{
		for (uint x=0;x<640;x++,dest++)
		{
			int	rv, gv, bv;
			int	shade = rand()%8-4;
			
			shade += (cos(sin((x+y)*3.14/140.0f)*3.14)*sin((float)(x+y)*3.14f/240.0f)*cos(y*3.14/120.0f))*6.0f;
			
			rv = (int)r+shade;
			gv = (int)g+shade;
			bv = (int)b+shade;
			
			if (spr && x >= 160 && x < 480 && y >= 80 && y < 400)
			{
				unsigned short	pxl = sprp[((y - 80)/10)*32 + 
					(x - 160)/10];
				if (pxl != 63519)
				{
					rv -= 16;
					gv -= 16;
					bv -= 16;
				}
			}
			
			if (rv < 0) rv = 0; else if (rv > 255) rv = 255;
			if (gv < 0) gv = 0; else if (gv > 255) gv = 255;
			if (bv < 0) bv = 0; else if (bv > 255) bv = 255;
			unsigned short	color = SDL_MapRGB(screen->format,
				rv, gv, bv);
			*dest = color;
		}
		dest += (screen->pitch/2)-640;
	}
	SDL_UnlockSurface(screen);

	for (uint i=0;i<width*height;i++)
	{
		if (!tile[i])
			continue;
		if (tile[i]->tclass->sprite->frames == 1)
			tile[i]->state->draw(tile[i]->x, tile[i]->y);
	}

	if (!staticPart)	
		staticPart = createSurface(640, 480, false);
	if (SDL_BlitSurface(screen, NULL, staticPart, NULL) != 0)
	{
		SDL_FreeSurface(staticPart);
		staticPart = NULL;
		SDL_Delay(10);
		drawStaticPart();
	}
}

void World::draw()
{
	if (!staticPart || refreshStaticPart)
	{
		refreshStaticPart = false;
		drawStaticPart();
	}
	else
	{
		if (SDL_BlitSurface(staticPart, NULL, screen, NULL) != 0)
		{
			SDL_FreeSurface(staticPart);
			staticPart = NULL;
			drawStaticPart();
		}
	
	}
	for (uint i=0;i<nonStaticTiles;i++)
		nonStaticTile[i]->state->draw(nonStaticTile[i]->x,
			nonStaticTile[i]->y);
	
	if (debugMode)
	{
		for (uint i=0;i<world.segments;i++)
		{
			SlashTDP::Segment	s = world.segment[i];
			drawLine(s.a.x, s.a.y, s.b.x, s.b.y,
				SDL_MapRGB(screen->format, 0, 255, 0));
		}
	}
	
	for (Object *obj=firstObject;obj;obj = obj->next)
		if (obj->visible)
		{
			obj->state->draw(obj->x, obj->y + obj->bob);
			if (!debugMode)
				continue;
			if (obj->body)
			{
				for (uint i=0;i<obj->body->segments;i++)
				{
					SlashTDP::Segment	s;
					s = obj->body->segment[i] +
						obj->body->center;
					drawLine(s.a.x, s.a.y, s.b.x, s.b.y,
						SDL_MapRGB(screen->format,
							255, 0, 255));
				}
			}
			drawBox(obj->x + obj->state->sprite->bx1,
				obj->y + obj->state->sprite->by1,
				obj->x + obj->state->sprite->bx2,
				obj->y + obj->state->sprite->by2,
				SDL_MapRGB(screen->format, 255, 255, 0));
			drawBox(obj->x, obj->y,
				obj->x + obj->state->sprite->frame[0]->image->w - 1,
				obj->y + obj->state->sprite->frame[0]->image->h - 1,
				SDL_MapRGB(screen->format, 0, 255, 255));
		}
}

void World::animate()
{
	for (uint i=0;i<nonStaticTiles;i++)
		nonStaticTile[i]->state->animate();
	Object	*next;
	for (Object *obj=firstObject;obj;obj = next)
	{
		next = obj->next;
		if (obj->visible)
		{
			if (obj->state->sprite->frames > 1)
				obj->state->animate();
			if (obj->oclass->bobbing)
			{
				obj->bob = (int)(sinf(obj->bobPos)*2.0f);
				obj->bobPos += 0.25;
			}
			if (obj->mc && !obj->deleteMC)
				obj->mc->step();
		}
	}
	world.move();
}

