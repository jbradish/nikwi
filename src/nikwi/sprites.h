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
** Nikwi Engine - Sprites
*/

#ifndef __SPRITES_H_INCLUDED__
#define __SPRITES_H_INCLUDED__

struct Frame
{
	SDL_Surface	*image;
	SDL_Surface	*mirror;	
	unsigned short	*localCopy;
	
	Frame();
	~Frame();
	
	void makeMirror();
	
	void draw(int x, int y, bool drawMirror);
};

struct Sprite
{
	String		name;
	Frame		**frame;
	Frame		*cframe;
	uint		frames;
	uint		delay;
	int		bx1, by1, bx2, by2;
	int		offsetX, offsetY;
	
	Sprite		*next;
	
	Sprite(String name);
	~Sprite();
	
	void addFrame(Frame *frame);
	void calcBoundingBox();
	
	bool collideWith(int x1, int y1, Sprite *spr, int x2, int y2);
};

struct SpriteState
{
	Sprite		*sprite;
	Frame		*frame;
	uint		index;
	uint		counter;
	bool		mirror;
	bool		paused;
	
	SpriteState(Sprite *sprite);
	~SpriteState();

	void draw(int x, int y);
	void animate();
};

extern Sprite	*cursor;

void initSprites();
void shutdownSprites();

Sprite *findSprite(String name);

#endif

