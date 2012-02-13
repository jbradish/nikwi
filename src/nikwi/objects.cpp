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
** Nikwi Engine - Objects
*/

#include "nikwi.h"

ObjectClass	**objectClass = NULL;
uint		objectClasses = 0;

//Script functions
static Object		*thisObject = NULL;
static Object		*otherObject = NULL;
static UScriptType	*stObject = NULL;
static UScriptType	*stTile = NULL;

static inline void runScript(UScript *script, Object *thisObj=NULL,
	Object *otherObj=NULL)
{
	if (script)
	{
		thisObject = thisObj;
		otherObject = otherObj;
		script->runExclusive();
	}
}

static void natMakeCode(UScriptVM *vm)
{
	String	code = vm->popStringArg();
	vm->a = (int)makeCode(code);
}

static void natGetThisObject(UScriptVM *vm)
{
	vm->pa = (void*)thisObject;
}

static void natGetOtherObject(UScriptVM *vm)
{
	vm->pa = (void*)otherObject;
}

static void natGetLevel(UScriptVM *vm)
{
	vm->a = game->level;
}

static void natGetHero(UScriptVM *vm)
{
	vm->pa = (void*)world->hero;
}

static void natCreateObject(UScriptVM *vm)
{
	int	y = vm->popIntArg();
	int	x = vm->popIntArg();
	uint	code = (uint)vm->popIntArg();
	vm->pa = (void*)world->createObject(code, x, y);
}

static void natCreateParticleObject(UScriptVM *vm)
{
	int	life = vm->popIntArg();
	float	f = vm->popFloatArg();
	float	a = vm->popFloatArg();
	int	y = vm->popIntArg();
	int	x = vm->popIntArg();
	uint	code = (uint)vm->popIntArg();
	vm->pa = (void*)world->createParticleObject(code, x, y, a, f, life);
}

static void natDestroyObject(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->deleteMe = true;
}

static void natDestroyObjectsOf(UScriptVM *vm)
{
	uint	code = (uint)vm->popIntArg();
	for (Object *obj=world->firstObject;obj;obj = obj->next)
		if (obj->oclass->code == code)
			obj->deleteMe = true;
}

static void natSetObjectSprite(UScriptVM *vm)
{
	String	spriteName = vm->popStringArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->setSprite(findSprite(spriteName)); 
}

static void natSetObjectLife(UScriptVM *vm)
{
	int	life = vm->popIntArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->life = life; 
}

static void natCreateMotionController(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->createMotionController();
}

static void natDestroyMotionController(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->deleteMC = true;
}

static void natCreatePhysicsController(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->createPhysicsController();
}

static void natSetObjectGravity(UScriptVM *vm)
{
	float	grav = vm->popFloatArg();
	Object	*obj = (Object*)vm->popPointerArg();
	if (obj->mc)
		obj->mc->gravity = grav;
	else if (obj->body)
		obj->body->gravity = grav;
}

static void natGetObjectOrientation(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->state->mirror;
}

static void natGetObjectXPosition(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->x;
}

static void natGetObjectYPosition(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->y;
}

static void natSetObjectPosition(UScriptVM *vm)
{
	int	y = vm->popIntArg();
	int	x = vm->popIntArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->x = x;
	obj->y = y;
	if (obj->mc)
	{
		obj->mc->x = x;
		obj->mc->y = y;
	}
}

static void natSetObjectMovement(UScriptVM *vm)
{
	float	y = vm->popFloatArg();
	float	x = vm->popFloatArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->mc->mx = x;
	obj->mc->my = y;
}

static void natSetObjectXMovement(UScriptVM *vm)
{
	float	v = vm->popFloatArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->mc->mx = v;
}

static void natSetObjectYMovement(UScriptVM *vm)
{
	float	v = vm->popFloatArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->mc->my = v;
}

static void natFlipObjectOrientation(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->state->mirror = !obj->state->mirror;
	if (obj->mc)
		obj->mc->mx = -obj->mc->mx;
	else if (obj->body)
		obj->body->velocity.x = -obj->body->velocity.x;
}

static void natFlipObjectVerticalOrientation(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	if (obj->mc)
		obj->mc->my = -obj->mc->my;
	else if (obj->body)
		obj->body->velocity.y = -obj->body->velocity.y;
}

static void natCopyObjectOrientation(UScriptVM *vm)
{
	Object	*src = (Object*)vm->popPointerArg();
	Object	*dst = (Object*)vm->popPointerArg();
	dst->state->mirror = src->state->mirror;
	if (dst->mc && src->mc)
	{
		dst->mc->mx = src->mc->mx;
	}
	else if (dst->body && src->body)
	{
		dst->body->velocity = src->body->velocity;
	}
}

static void natSetObjectSolidness(UScriptVM *vm)
{
	int	solid = vm->popIntArg();
	Object	*obj = (Object*)vm->popPointerArg();
	if (solid)
		obj->mc->solid = true;
	else
		obj->mc->solid = false;
}

static void natGetObjectSolidness(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->mc->solid;
}

static void natDisableObjectTileCollision(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->mc->disableTileCollision = true;
}

static void natEnableObjectTileCollision(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->mc->disableTileCollision = false;
}

static void natIsObjectTileCollisionEnabled(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->mc->disableTileCollision;
}
	
static void natSetObjectBounce(UScriptVM *vm)
{
	float	bounce = vm->popFloatArg();
	Object	*obj = (Object*)vm->popPointerArg();
	if (bounce == 0.0f)
	{
		if (obj->mc)
			obj->mc->bouncing = false;
		else if (obj->body)
			obj->body->bouncing = false;
	}
	else
	{
		if (obj->mc)
		{
			obj->mc->bouncing = true;
			obj->mc->bounce = bounce;
		}
		else if (obj->body)
		{
			obj->body->bouncing = true;
			obj->body->bounce = bounce;
		}
	}
}

static void natApplyForce(UScriptVM *vm)
{
	float	fy = vm->popFloatArg();
	float	fx = vm->popFloatArg();
	Object	*obj = (Object*)vm->popPointerArg();
	if (obj->mc)
	{
		obj->mc->applyForce(fx, fy);
	}
	else if (obj->body)
	{
		obj->body->applyForce(SlashTDP::Vector(fx, fy));
	}
}

static void natObjectOnGround(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->body->onGround;
}

static void natUnsetObjectOnGround(UScriptVM *vm)
{
	Object	*obj = (Object*)vm->popPointerArg();
	obj->body->onGround = false;
}

static void natSetObjectVar(UScriptVM *vm)
{
	int	value = vm->popIntArg();
	int	index = vm->popIntArg();
	Object	*obj = (Object*)vm->popPointerArg();
	obj->var[index] = value;
}

static void natGetObjectVar(UScriptVM *vm)
{
	int	index = vm->popIntArg();
	Object	*obj = (Object*)vm->popPointerArg();
	vm->a = obj->var[index];
}

static void natPutObjectOnTile(UScriptVM *vm)
{
	int	y = vm->popIntArg()%world->height;
	int	x = vm->popIntArg()%world->width;
	Object	*obj = (Object*)vm->popPointerArg();
	if (world->tile[world->width*y + x])
	{
		obj->y = y*32 - obj->state->sprite->frame[0]->image->h +
			world->tile[world->width*y + x]->tclass->sprite->by1;
	}
	else
	{
		obj->y = y*32 - obj->state->sprite->frame[0]->image->h;
	}
}

static void natCanObjectSeeObject(UScriptVM *vm)
{
	Object	*obj1 = (Object*)vm->popPointerArg();
	Object	*obj2 = (Object*)vm->popPointerArg();
	vm->a = false;
	if (abs(obj2->y - obj1->y) > 20)
		return;
	if (obj1->x > obj2->x && obj2->state->mirror)
		return;
	if (obj1->x < obj2->x && !obj2->state->mirror)
		return;
	vm->a = true;
}

static void natIncCandyCount(UScriptVM *vm)
{
	world->candyCount++;
}

static void natDecCandyCount(UScriptVM *vm)
{
	world->candyCount--;
	if (!world->candyCount)
	{
		float	a = 0.0;
		int	x = world->icecream->x + 16;
		int	y = world->icecream->y + 16;
		while (a < 360.0)
		{
			world->createParticleObject(makeCode("par3"), x, y, a,
				20.0, 60);
			a = a + 10.0;
		}

		world->icecream->visible = true;
		world->icecream->createMotionController();
		world->icecream->mc->gravity = 0.2f;
		world->icecream->mc->bouncing = true;
		world->icecream->mc->bounce = 0.8f;
	}
}

static void natPlaySample(UScriptVM *vm)
{
	uint	code = (uint)vm->popIntArg();
	playSample(findSample(code));
}

static void natLockHero(UScriptVM *vm)
{
	game->locked = true;
}

static void natUnlockHero(UScriptVM *vm)
{
	game->locked = false;
}

static void natLoseLife(UScriptVM *vm)
{
	game->loseLife();
}

static void natNextLevel(UScriptVM *vm)
{
	game->proceedToTheNextLevel = true;
}

//Object
Object::Object(uint code)
{
	mc = NULL;
	body = NULL;
	oclass = findObjectClass(code);
	state = new SpriteState(oclass->sprite);
	prev = next = NULL;
	w = state->frame->image->w;
	h = state->frame->image->h;
	bobPos = bob = 0;
	memset(var, 0, sizeof(var));
	deleteMe = false;
	deleteMC = false;
	visible = true;
	life = -1;
}

Object::~Object()
{
	if (mc)
		delete mc;
	if (body)
		world->world.removeBody(body);
	delete state;
}

void Object::setSprite(Sprite *sprite)
{
	bool	oldMirror = state->mirror;
	delete state;
	if (sprite)
		state = new SpriteState(sprite);
	else
		state = new SpriteState(oclass->sprite);
	state->mirror = oldMirror;
}

void Object::createMotionController()
{
	if (mc)
		delete mc;
	mc = new MotionController(this);
}

void Object::createPhysicsController()
{
	if (body)
		world->world.removeBody(body);
	int	w = oclass->sprite->bx2 - oclass->sprite->bx1;
	int	h = oclass->sprite->by2 - oclass->sprite->by1;
	body = world->world.createBody(SlashTDP::Vector(
		x + oclass->sprite->bx1,
		y + oclass->sprite->by1));
	body->addBox(0, 0, w, h);
}

void Object::init()
{
	life = -1;
	runScript(oclass->initScript, this);
}

bool Object::collision(Object *obj)
{
	runScript(oclass->collideScript, this, obj);
	return true;
}

void Object::timer()
{
	if (!visible)
		return;
	if (body)
	{
		x = (int)(body->center.x - oclass->sprite->bx1);
		y = (int)(body->center.y - oclass->sprite->by1);
		
		for (Object *ob=world->firstObject;ob;ob = ob->next)
		{
			if (ob == this || !ob->visible)
				continue;
			if (state->sprite->collideWith((int)x, (int)y,
				ob->state->sprite, ob->x, ob->y))
			{
				collision(ob);
				if (!ob->deleteMe && !ob->mc && !ob->body)
					ob->collision(this);
			}
		}
	}
	runScript(oclass->timerScript, this);
	if (life != -1)
	{
		life--;
		if (!life)
			deleteMe = true;
	}
}

bool Object::collision(Tile *tile)
{
	runScript(oclass->collideScript, this);
	return true;
}

//Globals

static UScript *getScript(String name)
{
	char	file[1024];
	String	code;
	UScript	*script;
	
	if (!name || !name[0])
		return NULL;
	sprintf(file, "data/scripts/%s.txt", name);
	code = getStringData(file);
	if (!code)
		return NULL;
	script = new UScript(code);
	if (!script->compile())
	{
		fprintf(stderr, "Error in script '%s'\n", file);
		delete script;
		return NULL;
	}
	
	return script;
}

void initObjects()
{
	stObject = usRegisterType("Object", USB_HANDLE);
	stTile = usRegisterType("Tile", USB_HANDLE);
	
	usRegisterIntConst("ORIENTATION_LEFT", 1);
	usRegisterIntConst("ORIENTATION_RIGHT", 0);
	
	usRegisterNativeFunc("makeCode", "s", natMakeCode, usInteger);
	
	usRegisterNativeFunc("getThisObject", "", natGetThisObject, stObject);
	usRegisterNativeFunc("getOtherObject", "", natGetOtherObject, stObject);
	usRegisterNativeFunc("getLevel", "", natGetLevel, usInteger);
	usRegisterNativeFunc("getHero", "", natGetHero, stObject);
	
	usRegisterNativeFunc("createObject", "iii", natCreateObject, stObject);
	usRegisterNativeFunc("createParticleObject", "iiiffi",
		natCreateParticleObject, stObject);
	usRegisterNativeFunc("destroyObject", "{Object}",natDestroyObject,NULL);
	usRegisterNativeFunc("destroyObjectsOf", "i", natDestroyObjectsOf,NULL);
	usRegisterNativeFunc("setObjectSprite", "{Object}s", natSetObjectSprite,
		NULL);
	usRegisterNativeFunc("setObjectLife", "{Object}i", natSetObjectLife,
		NULL);
	
	usRegisterNativeFunc("createMotionController", "{Object}",
		natCreateMotionController, NULL);
	usRegisterNativeFunc("destroyMotionController", "{Object}",
		natDestroyMotionController, NULL);
	usRegisterNativeFunc("createPhysicsController", "{Object}",
		natCreatePhysicsController, NULL);
	usRegisterNativeFunc("setObjectGravity", "{Object}f",
		natSetObjectGravity, NULL);
	usRegisterNativeFunc("getObjectOrientation", "{Object}",
		natGetObjectOrientation, usInteger);
	usRegisterNativeFunc("getObjectXPosition", "{Object}",
		natGetObjectXPosition, usInteger);
	usRegisterNativeFunc("getObjectYPosition", "{Object}",
		natGetObjectYPosition, usInteger);
	usRegisterNativeFunc("setObjectPosition", "{Object}ii",
		natSetObjectPosition, NULL);
	usRegisterNativeFunc("setObjectMovement", "{Object}ff",
		natSetObjectMovement, NULL);
	usRegisterNativeFunc("setObjectXMovement", "{Object}f",
		natSetObjectXMovement, NULL);
	usRegisterNativeFunc("setObjectYMovement", "{Object}f",
		natSetObjectYMovement, NULL);
	usRegisterNativeFunc("flipObjectOrientation", "{Object}",
		natFlipObjectOrientation, NULL);
	usRegisterNativeFunc("flipObjectVerticalOrientation", "{Object}",
		natFlipObjectVerticalOrientation, NULL);
	usRegisterNativeFunc("copyObjectOrientation", "{Object}{Object}",
		natCopyObjectOrientation, NULL);
	usRegisterNativeFunc("setObjectSolidness", "{Object}i",
		natSetObjectSolidness, NULL);
	usRegisterNativeFunc("getObjectSolidness", "{Object}",
		natGetObjectSolidness, usInteger);
	usRegisterNativeFunc("disableObjectTileCollision", "{Object}",
		natDisableObjectTileCollision, NULL);
	usRegisterNativeFunc("enableObjectTileCollision", "{Object}",
		natEnableObjectTileCollision, NULL);
	usRegisterNativeFunc("isObjectTileCollisionEnabled", "{Object}",
		natIsObjectTileCollisionEnabled, usInteger);
		
	usRegisterNativeFunc("setObjectBounce", "{Object}f",
		natSetObjectBounce, NULL);
	usRegisterNativeFunc("applyForce", "{Object}ff",
		natApplyForce, NULL);
	usRegisterNativeFunc("objectOnGround", "{Object}",
		natObjectOnGround, usInteger);
	usRegisterNativeFunc("unsetObjectOnGround", "{Object}",
		natUnsetObjectOnGround, NULL);
		
	usRegisterNativeFunc("setObjectVar", "{Object}ii",
		natSetObjectVar, NULL);
	usRegisterNativeFunc("getObjectVar", "{Object}i",
		natGetObjectVar, usInteger);
		
	usRegisterNativeFunc("putObjectOnTile", "{Object}ii",
		natPutObjectOnTile, NULL);
	usRegisterNativeFunc("canObjectSeeObject", "{Object}{Object}",
		natCanObjectSeeObject, usInteger);

	usRegisterNativeFunc("incCandyCount", "", natIncCandyCount, NULL);
	usRegisterNativeFunc("decCandyCount", "", natDecCandyCount, NULL);
	
	usRegisterNativeFunc("playSample", "i", natPlaySample, NULL);

	usRegisterNativeFunc("lockHero", "", natLockHero, NULL);
	usRegisterNativeFunc("unlockHero", "", natUnlockHero, NULL);
	usRegisterNativeFunc("loseLife", "", natLoseLife, NULL);
	usRegisterNativeFunc("nextLevel", "", natNextLevel, NULL);
	
	BadCFGNode	*objects = loadCFG("data/objects.txt");
	
	for (BadCFGNode *obj=objects->child;obj;obj = obj->next)
	{
		ObjectClass	*oc = new ObjectClass;
		oc->sprite = findSprite(getBadCFGValue(obj, "sprite",
			obj->value));
		oc->code = makeCode(getBadCFGValue(obj, "code", "????"));
		oc->bobbing = getBadCFGValue(obj, "bobbing", "")[0] == 'y';
		oc->initScript = getScript(getBadCFGValue(obj,
			"scripts.init", ""));
		oc->collideScript = getScript(getBadCFGValue(obj,
			"scripts.collide", ""));
		oc->timerScript = getScript(getBadCFGValue(obj,
			"scripts.timer", ""));
		objectClass = (ObjectClass**)realloc(objectClass,
			sizeof(ObjectClass*)*(objectClasses + 1));
		objectClass[objectClasses++] = oc;
	}
}

void shutdownObjects()
{
	for (uint i=0;i<objectClasses;i++)
	{
		if (objectClass[i]->timerScript)
			delete objectClass[i]->timerScript;
		if (objectClass[i]->collideScript)
			delete objectClass[i]->collideScript;
		if (objectClass[i]->initScript)
			delete objectClass[i]->initScript;
		delete objectClass[i];
	}
	free(objectClass);
}

ObjectClass *findObjectClass(uint code)
{
	for (uint i=0;i<objectClasses;i++)
		if (objectClass[i]->code == code)
			return objectClass[i];
	return NULL;
}

