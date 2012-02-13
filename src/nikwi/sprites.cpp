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

#include "nikwi.h"

static Sprite	*spriteBank = NULL;

Sprite		*cursor = NULL;

//Frame
Frame::Frame()
{
	image = mirror = NULL;
	localCopy = NULL;
}

Frame::~Frame()
{
	if (mirror)
		SDL_FreeSurface(mirror);
	SDL_FreeSurface(image);
	free(localCopy);
}

void Frame::makeMirror()
{
	if (mirror)
		SDL_FreeSurface(mirror);
	mirror = createSurface(image->w, image->h);
	SDL_LockSurface(mirror);
	SDL_LockSurface(image);
	for (int y=0;y<image->h;y++)
	{
		unsigned short	*src =
			&((unsigned short*)image->pixels)[y*image->pitch/2];
		unsigned short	*dst =
			&((unsigned short*)mirror->pixels)[y*mirror->pitch/2]
			+ (image->w - 1);
		for (int x=0;x<image->w;x++,src++,dst--)
			*dst = *src; 
	}
	SDL_UnlockSurface(image);
	SDL_UnlockSurface(mirror);
}

void Frame::draw(int x, int y, bool drawMirror)
{
	SDL_Rect	ir, dr;
	SDL_Surface	*img = drawMirror?mirror:image;
	ir.x = ir.y = 0;
	ir.w = img->w;
	ir.h = img->h;
	dr.x = x;
	dr.y = y;
	if (SDL_BlitSurface(img, &ir, screen, &dr) == -2)
	{
		while (SDL_LockSurface(image) < 0)
			SDL_Delay(10);
		memcpy(image->pixels, localCopy, image->w*image->h*2);
		SDL_UnlockSurface(image);
		makeMirror();
		
		draw(x, y, drawMirror);
	}
}


//Sprite
Sprite::Sprite(String name)
{
	this->name = strdup(name);
	frame = NULL;
	frames = 0;
	delay = 1;
}

Sprite::~Sprite()
{
	free(name);
	
	if (frame)
	{
		for (uint i=0;i<frames;i++)
			delete frame[i];
		free(frame);
	}
}

void Sprite::addFrame(Frame *frame)
{
	this->frame = (Frame**)realloc(this->frame, sizeof(Frame*)*(frames+1));
	this->frame[frames++] = frame;
}

void Sprite::calcBoundingBox()
{
	bool		topFound = false, leftFound = false;
	unsigned short	*src;
	
	bx1 = by1 = bx2 = by2 = 0;
	
	if (!frame[0] || !frame[0]->image)
		return;
	
	if (SDL_MUSTLOCK(frame[0]->image))
		SDL_LockSurface(frame[0]->image);
	for (int y=0;y<frame[0]->image->h;y++)
	{
		bool	hasPixels = false;
		src = &((unsigned short*)frame[0]->image->pixels)[y*
			frame[0]->image->pitch/2];
		for (int x=0;x<frame[0]->image->w;x++,src++)
		{
			if (*src != SDL_MapRGB(frame[0]->image->format, 248, 0,
				248))
			{
				hasPixels = true;
			}
		}
		if (hasPixels)
		{
			if (!topFound)
			{
				by1 = y;
				topFound = true;
			}
			by2 = y;
		}
	}

	for (int x=0;x<frame[0]->image->w;x++)
	{
		bool		hasPixels = false;
		unsigned short	slice;
		for (int y=0;y<frame[0]->image->h;y++)
		{
			slice = ((unsigned short*)frame[0]->image->pixels)[y*
				frame[0]->image->pitch/2 + x];
			if (slice != SDL_MapRGB(frame[0]->image->format, 248,0,
				248))
			{
				hasPixels = true;
				break;
			}
		}
		if (hasPixels)
		{
			if (!leftFound)
			{
				bx1 = x;
				leftFound = true;
			}
			bx2 = x;
		}
	}
	if (SDL_MUSTLOCK(frame[0]->image))
		SDL_UnlockSurface(frame[0]->image);
}

bool Sprite::collideWith(int x1, int y1, Sprite *spr, int x2, int y2)
{
	return rectOverRect(x1 + bx1 - offsetX, y1 + by1 - offsetY,
		x1 + bx2 - offsetX, y1 + by2 - offsetY,
		x2 + spr->bx1 - spr->offsetX, y2 + spr->by1 - spr->offsetY,
		x2 + spr->bx2 - spr->offsetX, y2 + spr->by2 - spr->offsetY);
}

//SpriteState
SpriteState::SpriteState(Sprite *sprite)
{
	this->sprite = sprite;
	frame = sprite->frame[index = 0];
	counter = 0;
	mirror = false;
	paused = false;
}

SpriteState::~SpriteState()
{
}

void SpriteState::draw(int x, int y)
{
	frame->draw(x - sprite->offsetX, y - sprite->offsetY, mirror);
}

void SpriteState::animate()
{
	if (paused)
		return;
	counter++;
	if (counter == sprite->delay)
	{
		counter = 0;
		index++;
		if (index == sprite->frames)
			index = 0;
		frame = sprite->frame[index];
	}
}


//Globals
static Frame *loadFrame(String file)
{
	SDL_Surface	*surf = loadImage(file);
	Frame		*frame;
	if (!surf)
	{
		//printf("FAiled: %s\n", file);
		return NULL;
	}
	
	frame = new Frame;
	frame->image = surf;
	frame->localCopy = (unsigned short*)malloc(surf->w*surf->h*2);
	SDL_LockSurface(surf);
	memcpy(frame->localCopy, surf->pixels, surf->w*surf->h*2);
	SDL_UnlockSurface(surf);
	frame->makeMirror();
	
	return frame;
}

static void loadSpriteDirectory(BadCFGNode *dir)
{
	char	buff[16384];
	
	for (BadCFGNode *spr=dir->child;spr;spr=spr->next)
	{
		Sprite	*sprite = new Sprite(spr->value);
		Frame	*frame;
		
		sprite->delay = atoi(getBadCFGValue(spr, "delay", "1"));
		sprite->offsetX = atoi(getBadCFGValue(spr, "offsetX", "0"));
		sprite->offsetY = atoi(getBadCFGValue(spr, "offsetY", "0"));
		sprite->next = spriteBank;
		spriteBank = sprite;
		
		if (getBadCFGValue(spr, "frame", "")[0])
		{
			for (BadCFGNode *fr=spr->child;fr;fr=fr->next)
			{
				if (strcmp(fr->name, "frame"))
					continue;
				sprintf(buff, "data/%s/%s.ut", dir->value,
					fr->value);
				frame = loadFrame(buff);
				if (frame)
					sprite->addFrame(frame);
			}
		}
		else
		{
			sprintf(buff, "data/%s/%s.ut", dir->value, spr->value);
			frame = loadFrame(buff);
			if (frame)
				sprite->addFrame(frame);
		}
		
		sprite->calcBoundingBox();
		
		sprite->by1 += atoi(getBadCFGValue(spr, "collTop", "0"));
	}
}

void initSprites()
{
	BadCFGNode	*sprites = loadCFG("data/sprites.txt");
	if (!sprites)
		fatal("Cannot load sprites configuration file");
	
	for (BadCFGNode *dir=sprites->child->child;dir;dir=dir->next)
		loadSpriteDirectory(dir);
	
	destroyBadCFGNode(sprites);
	
	cursor = findSprite("cursor");
}

void shutdownSprites()
{
	while (spriteBank)
	{
		Sprite	*next = spriteBank->next;
		delete spriteBank;
		spriteBank = next;
	}
}

Sprite *findSprite(String name)
{
	for (Sprite *spr=spriteBank;spr;spr=spr->next)
		if (!strcmp(name, spr->name))
			return spr;
	return NULL;
}

