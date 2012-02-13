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

#ifndef __MOTION_H_INCLUDED__
#define __MOTION_H_INCLUDED__

#define GRAVITY		0.5f

struct MotionController
{
	Object		*obj;
	float		x, y;
	float		vx, vy;
	float		mx, my;
	float		gravity;
	float		bounce;
	bool		bouncing;
	bool		onGround;
	bool		solid;
	bool		disableTileCollision;
	
	MotionController(Object *obj);
	~MotionController();
	
	Tile *collides();
	void step();
	
	void applyForce(float fx, float fy);
};

#endif

