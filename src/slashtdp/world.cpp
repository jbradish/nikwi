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

//World
World::World()
{
	segment = NULL;
	segments = 0;
	firstBody = lastBody = NULL;
	resistance.x = 0.9f;
	resistance.y = 0.99f;
}

World::~World()
{
	newWorld();
}

void World::newWorld()
{
	removeAllSegments();
	removeAllBodies();
	segment = NULL;
	segments = 0;
	firstBody = lastBody = NULL;
}

void World::addSegment(const Segment &seg)
{
	segment = (Segment*)realloc(segment, sizeof(Segment)*(segments + 1));
	segment[segments] = seg;
	segment[segments++].calcNAndC();
}

void World::addBox(float x1, float y1, float x2, float y2)
{
	addSegment(Segment(x1, y1, x2, y1));
	addSegment(Segment(x2, y1, x2, y2));
	addSegment(Segment(x2, y2, x1, y2));
	addSegment(Segment(x1, y2, x1, y1));
}

void World::removeSegment(uint index)
{
	for (uint i=index;i<segments-1;i++)
		segment[i] = segment[i + 1];
	segments--;
	segment = (Segment*)realloc(segment, sizeof(Segment)*segments);
}

void World::removeAllSegments()
{
	if (segment)
		free(segment);
	segment = NULL;
	segments = 0;
}

bool World::optimizeSegment(uint index)
{
	float	slope1a = (segment[index].a.x - segment[index].b.x)/
		(segment[index].a.y - segment[index].b.y);
	float	slope1b = (segment[index].a.y - segment[index].b.y)/
		(segment[index].a.x - segment[index].b.x);

	for (uint i=0;i<segments;i++)
	{
		if (i == index)
			continue;
		
		float	slope2a = (segment[i].a.x - segment[i].b.x)/
			(segment[i].a.y - segment[i].b.y);
		float	slope2b = (segment[i].a.y - segment[i].b.y)/
			(segment[i].a.x - segment[i].b.x);
		bool	sameSlope = (slope1a == slope2a || slope1b == slope2b);

		
		if (sameSlope && 
			segment[index].a.distanceSquared(segment[i].b) < 2.25f)
		{
			segment[index].a = segment[i].a;
			removeSegment(i);
			return true;
		}
		/*
		if (sameSlope &&
			(segment[index].a.distanceSquared(segment[i].a) < 2.25f
			||segment[index].b.distanceSquared(segment[i].b) < 2.25f
			))
		{
			float	l1 = segment[index].lengthSquared();
			float	l2 = segment[i].lengthSquared();
			if (l1 == l2)
				continue;
			if (l1 < l2)
				removeSegment(index);
			else
				removeSegment(i);
			return true;
		}
		
		*/
		if (segment[index].a.distanceSquared(segment[i].a) < 2.25f &&
			segment[index].b.distanceSquared(segment[i].b) < 2.25f)
		{
			removeSegment(i);
			removeSegment(index);
			return true;
		}

		if (segment[index].a.distanceSquared(segment[i].b) < 2.25f &&
			segment[index].b.distanceSquared(segment[i].a) < 2.25f)
		{
			removeSegment(i);
			removeSegment(index);
			return true;
		}
	}
	
	return false;
}

void World::optimizationCycle()
{
	for (int i=0;i<(int)segments;i++)
		if (optimizeSegment(i))
			i = -1;
	for (int i=0;i<(int)segments;i++)
		if (segment[i].lengthSquared() < 2.25f)
		{
			removeSegment(i);
			i = -1;
		}
	
	for (int i=0;i<(int)segments;i++)
		for (int j=0;j<(int)segments;j++)
		{
			if (i == j)
				continue;
			if (segment[i].a.distanceSquared(segment[j].a) < 4.25f
				&& segment[i].a.distanceSquared(segment[j].a) >
					0)
			{
				segment[i].a = segment[j].a;
				i = -1;
				break;
			}
			if (segment[i].b.distanceSquared(segment[j].b) < 4.25f
				&& segment[i].b.distanceSquared(segment[j].b) >
					0)
			{
				segment[i].b = segment[j].b;
				i = -1;
				break;
			}
			/*
			if (segment[i].a.distanceSquared(segment[j].b) < 4.25f
				&& segment[i].a.distanceSquared(segment[j].b) > 
					0)
			{
				segment[i].a = segment[j].b;
				i = -1;
				break;
			}
			if (segment[i].b.distanceSquared(segment[j].a) < 4.25f
				&& segment[i].b.distanceSquared(segment[j].a) > 
					0)
			{
				segment[i].b = segment[j].a;
				i = -1;
				break;
			}
			*/
		}
}

void World::optimize()
{
	//printf("Segments before optimization = %i\n", segments);
	
	for (;;)
	{
		uint	lastSegments = segments;
		optimizationCycle();
		if (lastSegments == segments)
			break;
	}
	
	for (uint i=0;i<segments;i++)
		segment[i].calcNAndC();
	
//	printf("Segments after optimization = %i\n", segments);
}

Body *World::createBody(const Vector &c)
{
	Body	*body = new Body(c);
	addBody(body);
	return body;
}

void World::addBody(Body *body)
{
	body->world = this;
	body->prev = lastBody;
	body->next = NULL;
	if (lastBody)
		lastBody->next = body;
	else
		firstBody = body;
	lastBody = body;
}

void World::removeBody(Body *body)
{
	if (body->next)
		body->next->prev = body->prev;
	else
		lastBody = body->prev;
	if (body->prev)
		body->prev->next = body->next;
	else
		firstBody = body->next;
	delete body;
}

void World::removeAllBodies()
{
	while (firstBody)
	{
		lastBody = firstBody->next;
		delete firstBody;
		firstBody = lastBody;
	}
}

void World::move()
{
	for (Body *body=firstBody;body;body = body->next)
		body->move();
}

