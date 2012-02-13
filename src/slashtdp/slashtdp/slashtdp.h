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

#ifndef __SLASHTDP_H_INCLUDED__
#define __SLASHTDP_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#if defined(WIN32) || defined(__APPLE__)
typedef unsigned int	uint;
#endif

namespace SlashTDP
{
	#define SQR(x)	((x)*(x))
	
	struct Vector
	{
		float	x;
		float	y;
		
		inline Vector()
		{
			x = y = 0.0f;
		}
		
		inline Vector(const Vector &ref)
		{
			x = ref.x;
			y = ref.y;
		}
		
		inline Vector(float x, float y)
		{
			this->x = x;
			this->y = y;
		}
		
		inline float length()
		{
			return hypotf(x, y);
		}
		
		inline float lengthSquared()
		{
			return SQR(x) + SQR(y);
		}

		inline void normalize()
		{
			float	len = length();
			if (len != 0.0f)
			{
				x /= len;
				y /= len;
			}
		}
		
		inline Vector normalized()
		{
			float	len = length();
			if (len == 0.0f)
				return Vector(0.0f, 0.0f);
			else
				return Vector(x/len, y/len);
		}
		
		inline Vector perpendicular()
		{
			return Vector(-y, x);
		}
		
		inline Vector reflected(const Vector &n)
		{
			float	d = (*this)*n;
			return Vector(x - 2*n.x*d,
				y - 2*n.y*d);
		}
		
		inline Vector rotated(float a)
		{
			return Vector(cosf(a)*x - sinf(a)*y,
				sinf(a)*x + cosf(a)*y);
		}
		
		inline Vector projected(const Vector &ref)
		{
			float	d1 = ref.x==0.0f?ref.y:(ref.y/ref.x);
			float	d2 = ref.y==0.0f?ref.x:(ref.x/ref.y);
			float	d = d1 + d2;
			float	Px = (x*d2 + y)/d;
			
			return Vector(Px, d1*Px);
		}
		
		inline float distance(const Vector &ref)
		{
			return hypot(x - ref.x, y - ref.y);
		}
		
		inline float distanceSquared(const Vector &ref)
		{
			return SQR(x - ref.x)+SQR(y - ref.y);
		}

		inline Vector operator + (const Vector &ref)
		{
			return Vector(x + ref.x, y + ref.y);
		}
		
		inline Vector operator - (const Vector &ref)
		{
			return Vector(x - ref.x, y - ref.y);
		}
		
		inline float operator * (const Vector &ref)
		{
			return x*ref.x + y*ref.y;
		}
		
		inline Vector operator * (const float scalar)
		{
			return Vector(x*scalar, y*scalar);
		}
		
		inline Vector operator / (const float scalar)
		{
			return Vector(x/scalar, y/scalar);
		}

		inline void operator += (const Vector &ref)
		{
			x += ref.x;
			y += ref.y;
		}
		
		inline void operator -= (const Vector &ref)
		{
			x -= ref.x;
			y -= ref.y;
		}
		
		inline void operator *= (const float scalar)
		{
			x *= scalar;
			y *= scalar;
		}

		inline void operator /= (const float scalar)
		{
			x /= scalar;
			y /= scalar;
		}
	};
	
	struct Segment
	{
		Vector	a;
		Vector	b;
		Vector	n;
		Vector	c;
		
		inline Segment()
		{
		}
		
		inline Segment(const Segment &ref)
		{
			a = ref.a;
			b = ref.b;
		}
		
		inline Segment(const Vector &a, const Vector &b)
		{
			this->a = a;
			this->b = b;
		}
		
		inline Segment(float x1, float y1, float x2, float y2)
		{
			a = Vector(x1, y1);
			b = Vector(x2, y2);
		}
		
		inline void calcNAndC()
		{
			n = (b - a).perpendicular().normalized();
			c = a + (b - a)*0.5f;
		}
		
		inline float length()
		{
			return b.distance(a);
		}
		
		inline float lengthSquared()
		{
			return b.distanceSquared(a);
		}
		
		inline float distance(const Vector &p)
		{
			float	dx = b.x - a.x;
			float	dy = b.y - a.y;
			float	u = ((p.x - a.x)*dx + (p.y - a.y)*dy)/
				lengthSquared();
			return Vector(a.x + u*dx, a.y + u*dy).distance(p);
		}

		inline float distanceSquared(const Vector &p)
		{
			float	dx = b.x - a.x;
			float	dy = b.y - a.y;
			float	u = ((p.x - a.x)*dx + (p.y - a.y)*dy)/
				lengthSquared();
			return Vector(a.x + u*dx,a.y + u*dy).distanceSquared(p);
		}

		inline bool intersect(const Segment &ref, Vector &p)
		{
			float	dx = b.x - a.x;
			float	dy = b.y - a.y;
			float	n;
			float	d =  (ref.b.y - ref.a.y)*dx -
				(ref.b.x - ref.a.x)*dy;
			float	u, u2;
			
			n = (ref.b.x - ref.a.x)*(a.y - ref.a.y) -
				(ref.b.y - ref.a.y)*(a.x - ref.a.x);
			
			if (d == 0.0f && n == 0.0f)
			{
				p = a;
				return true;
			}
			
			if (d == 0.0f)
				return false;
			
			u = n/d;
			if (u < 0.0f || u > 1.0f)
				return false;
			
			u2 = (dx*(a.y - ref.a.y) - dy*(a.x - ref.a.x))/d;
			if (u2 < 0.0f || u2 > 1.0f)
				return false;
			
			p.x = a.x + u*dx;
			p.y = a.y + u*dy;
			
			return true;
		}
		
		inline Vector projected(const Vector &p)
		{
			return (b - a).projected(p);
		}
		
		inline Segment operator + (const Segment &ref)
		{
			return Segment(a + ref.a, b + ref.b);
		}

		inline Segment operator + (const Vector &ref)
		{
			return Segment(a + ref, b + ref);
		}
	};

	struct Body
	{
		struct World	*world;
		
		float		gravity;
		
		int		resting;
		bool		bouncing;
		float		bounce;
		bool		blockOthers;
		Vector		center;
		Vector		velocity;
		bool		onGround;
		Segment		collSegment;
		Body		*bodyColl;
		
		Vector		*point;
		uint		points;
		Segment		*segment;
		uint		segments;
		
		Body		*prev;
		Body		*next;
		
		Body();
		Body(const Vector &c);
		~Body();
		
		void addPoint(const Vector &p);
		void addSegment(const Segment &seg);
		void addBox(float x1, float y1, float x2, float y2);
		
		void applyForce(const Vector &f);
		
		bool intersects(Vector vel);
		void move();
	};
	
	struct World
	{
		Segment		*segment;
		uint		segments;
		Vector		resistance;
		
		Body		*firstBody;
		Body		*lastBody;
		
		World();
		~World();
		
		void newWorld();
		
		void addSegment(const Segment &seg);
		void addBox(float x1, float y1, float x2, float y2);
		void removeSegment(uint index);
		void removeAllSegments();
		
		bool optimizeSegment(uint index);
		void optimizationCycle();
		void optimize();

		Body *createBody(const Vector &c);
		void addBody(Body *body);
		void removeBody(Body *body);
		void removeAllBodies();
		
		void move();
	};
	
};

#endif

