/*
 * Nikwi Deluxe - SlashTDP
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

#include <slashtdp/slashtdp.h>

using namespace SlashTDP;

//Body
Body::Body()
{
	gravity = 0.1f;
	world = NULL;
	point = NULL;
	prev = next = NULL;
	points = 0;
	segment = NULL;
	segments = 0;
	onGround = false;
	blockOthers = false;
	resting = 0;
	bounce = false;
	bouncing = 0.0f;
}

Body::Body(const Vector &c)
{
	gravity = 0.1f;
	world = NULL;
	center = c;
	point = NULL;
	prev = next = NULL;
	points = 0;
	segment = NULL;
	segments = 0;
	onGround = false;
	blockOthers = false;
	resting = 0;
	bounce = false;
	bouncing = 0.0f;
}

Body::~Body()
{
	if (point)
		free(point);
	if (segment)
		free(segment);
}

void Body::addPoint(const Vector &p)
{
	for (uint i=0;i<points;i++)
		if (p.x == point[i].x && p.y == point[i].y)
			return;
	point = (Vector*)realloc(point, sizeof(Vector)*(points + 1));
	point[points++] = p;
}

void Body::addSegment(const Segment &seg)
{
	segment = (Segment*)realloc(segment, sizeof(Segment)*(segments + 1));
	segment[segments] = seg;
	segment[segments++].calcNAndC();
	addPoint(seg.a);
	addPoint(seg.b);
}

void Body::addBox(float x1, float y1, float x2, float y2)
{
	addSegment(Segment(x1, y1, x2, y1));
	addSegment(Segment(x2, y1, x2, y2));
	addSegment(Segment(x2, y2, x1, y2));
	addSegment(Segment(x1, y2, x1, y1));
}

void Body::applyForce(const Vector &f)
{
	resting = false;
	velocity += f;
}

bool Body::intersects(Vector vel)
{
	Vector	p;
	
	bodyColl = NULL;
	for (uint j=0;j<segments;j++)
	{
		for (uint i=0;i<world->segments;i++)
		{
			if (vel*world->segment[i].n < 0.0f)
				continue;
			if (world->segment[i].intersect(segment[j] + center +
				vel, p))
			{
				
				collSegment = world->segment[i];
				collSegment.c = world->segment[i].c;
				collSegment.n = world->segment[i].n;
				bodyColl = NULL;
				return true;
			}
		}
		
		for (Body *b=world->firstBody;b;b=b->next)
		{
			if (!b->blockOthers)
				continue;
			if (b == this || !b->blockOthers)
				continue;
			for (uint k=0;k<b->segments;k++)
			{
				if (vel*b->segment[k].n < 0.0f)
					continue;
				if ((b->segment[k] + b->center).intersect(
					segment[j] + center + vel, p))
				{
					collSegment = b->segment[k] + b->center;
					collSegment.c = b->segment[k].c +
						b->center;
					collSegment.n = b->segment[k].n;
					bodyColl = b;
					return true;
				}
			}
		}
	}
	return false;
}

void Body::move()
{
	int	i, trials = 0;
	bool	collision = false;
	
	if (resting > 30)
		return;
	
	velocity *= world->resistance.y;
	velocity.x *= world->resistance.x;
	velocity.y += gravity;
	
	if (fabs(velocity.x) < 0.15f)
		velocity.x = 0.0f;

	do
	{
		Vector	vel = velocity;
		
		for (i=0;i<256;i++)
			if (intersects(velocity))
			{
				collision = true;
				velocity *= 0.5f;
			}
			else
			{
				if (i > 0)
					center -= velocity*0.15f;
				break;
			}
		
		if (i == 255)
		{
			velocity = vel - collSegment.n;
			return;	//failed//
		}
	
		if (collision)
		{
			float	dist = 1000000000.0f;
			int	cseg = -1;
			Vector	uv = velocity*100000.0;
			
			if (!bodyColl)
				for (uint j=0;j<points;j++)
				{
					Segment	ray(point[j] + center,
						point[j] + center + uv);
					Vector	rip;
					for (uint w=0;w<world->segments;w++)
					{
						if (ray.intersect(
							world->segment[w], rip))
						{
							float d = rip.
								distanceSquared(
									point[j]
								+ center);
							if (d < dist)
							{
								dist = d;
								cseg = w;
							}
						}
					}
				}
			
			if (!bodyColl && cseg == -1)
			{
				center -= collSegment.n;
			}
			else
			{
				if (!bodyColl)
				{
					collSegment = world->segment[cseg];
					collSegment.n = world->segment[cseg].n;
					collSegment.c = world->segment[cseg].c;
				}
				//Vector	n = collSegment.n;
				//if (fabs(n.y) > fabs(n.x) && n.y > 0.0f)
				//	onGround = true;
				collision = true;
				velocity = vel.projected(collSegment.b - 
					collSegment.a);
				if (velocity.y == 0.0f && vel.y > 0.0f)
				{
					onGround = true;
					for (float oy=0.0f;oy<32.0f;oy+=0.5f)
						if (!intersects(Vector(0, oy)))
							velocity.y = oy;
						else
							break;
				}
				if (bouncing)
				{
					center += velocity;
					velocity = vel.reflected(collSegment.n)
						*bounce;
				}
			}
		}
			
		trials++;
		if (trials == 1)
			break;
	}
	while (!collision);

	if (!intersects(velocity))
		center += velocity;

	if (velocity.lengthSquared() < 0.001f)
		resting++;
}
























