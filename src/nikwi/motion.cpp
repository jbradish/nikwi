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
** Nikwi Engine - Motion controllers
*/

#include <math.h>
#include "nikwi.h"

//MotionController
MotionController::MotionController(Object *obj)
{
	this->obj = obj;
	x = obj->x;
	y = obj->y;
	vx = vy = mx = my = 0;
	onGround = false;
	gravity = GRAVITY;
	bounce = 0.0f;
	bouncing = false;
	solid = true;
	disableTileCollision = false;
}

MotionController::~MotionController()
{
}

Tile *MotionController::collides()
{
	uint	x1, y1, x2, y2;
	
	x1 = (int)x >> 5;
	y1 = (int)y >> 5;
	x2 = ((int)(x + obj->w) >> 5) + 2;
	y2 = ((int)(y + obj->h) >> 5) + 2;
	
	if (x2 > world->width)
		x2 = world->width;
	if (y2 > world->height)
		y2 = world->height;
	
	for (uint y=y1;y<y2;y++)
		for (uint x=x1;x<x2;x++)
		{
			Tile	*tile = world->tile[y*world->width + x];
			
			if (!tile)
				continue;
			if (!tile->tclass->sprite->collideWith(tile->x,
				tile->y, obj->oclass->sprite,
				(int)this->x, (int)this->y))
				continue;
			
			return tile;
		}
		
	return NULL;
}

void MotionController::step()
{
	float	ox = x, oy = y;
	vy += gravity;
	
	if (!solid)
	{
		x += vx + mx;
		y += vy + my;
		obj->x = (int)x;
		obj->y = (int)y;
		return;
	}
	
	if (disableTileCollision)
	{
		x += vx + mx;
		y += vy + my;
		goto skipTileCollisionCheck;
	}

	if (x + vx + mx < 0)
	{
		obj->collision((Tile*)NULL);
		obj->x = (int)(x = 0);
		vx = 0;
	}
	if (y + vy + my < 0)
	{
		obj->collision((Tile*)NULL);
		obj->y = (int)(y = 0);
		vy = 0;
		if (obj == world->hero)
			game->jumping = false;
	}
	if (x + vx + mx + obj->w > 639)
	{
		obj->collision((Tile*)NULL);
		obj->x = (int)(x = 639-obj->w);
		vx = 0;
	}
	if (y + vy + my + obj->h > 479)
	{
		obj->collision((Tile*)NULL);
		obj->y = (int)(y = 479-obj->h);
		vy = 0;
	}
	
	Tile	*ctile, *rtile;
	float	ax, ay;
	int	cycle;

	ax = vx + mx;
	ay = vy + my;
	
	vx = vx*0.8f;
	
	x += ax;
	rtile = NULL;
	ctile = collides();
	cycle = 0;
	while (ctile && ++cycle < 15)
	{
		if (ax < 0)
			x = ctile->x - obj->oclass->sprite->bx1
				+ ctile->tclass->sprite->bx2 + 1;
		else
			x = ctile->x - obj->oclass->sprite->bx2 - 1
				+ ctile->tclass->sprite->bx1;
		
		if (bouncing)
			vx = -vx*bounce;
		else
			vx = 0;
		rtile = ctile;
		ctile = collides();
	}
	
	y += ay;
	ctile = collides();
	if (!ctile && (int)y > obj->y)
		onGround = false;
	cycle = 0;
	while (ctile && ++cycle < 15)
	{
		if (ay < 0)
			y = ctile->y - obj->oclass->sprite->by1
				+ ctile->tclass->sprite->by2 + 1;
		else
		{
			onGround = true;
			y = ctile->y - obj->oclass->sprite->by2 - 1
				+ ctile->tclass->sprite->by1;
		}
		
		vx *= 0.5f;
		if (bouncing)
			vy = -vy*bounce;
		else
			vy = 0;
		if (obj == world->hero)
			game->jumping = false;
		rtile = ctile;
		ctile = collides();
	}
	
	if (rtile)
		obj->collision(rtile);
	
	skipTileCollisionCheck:
	Object	*next;
	for (Object *ob=world->firstObject;ob;ob = next)
	{
		next = ob->next;
		if (ob == obj || !ob->visible)
			continue;
		if (obj->state->sprite->collideWith((int)x, (int)y,
			ob->state->sprite, ob->x, ob->y))
		{
			obj->collision(ob);
			if (!ob->deleteMe && !ob->mc && !ob->body)
				ob->collision(obj);
		}
	}
	
	obj->x = (int)x;
	obj->y = (int)y;
}

void MotionController::applyForce(float fx, float fy)
{
	vx += fx;
	vy += fy;
}

