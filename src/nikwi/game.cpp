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
** Nikwi Engine - Game screen
*/

#include "nikwi.h"

Game	*game = NULL;

//Game
Game::Game()
{
	gti = findSprite("gti");
	died = findSprite("died");
	pwdBack = findSprite("pwd");
	letter['a'-'a'] = findSprite("a");
	letter['c'-'a'] = findSprite("c");
	letter['f'-'a'] = findSprite("f");
	letter['g'-'a'] = findSprite("g");
	letter['n'-'a'] = findSprite("n");
	letter['q'-'a'] = findSprite("q");
	letter['x'-'a'] = findSprite("x");

	memset(levelPwd, 0, sizeof(levelPwd));
	levelPwd[2] = "cafqnx";
	levelPwd[5] = "xaafnn";
	levelPwd[8] = "naaxxg";
	levelPwd[11] = "gagqxf";
	levelPwd[14] = "xcfgqx";
	levelPwd[17] = "fqxgnx";
	levelPwd[20] = "naxgax";
	levelPwd[23] = "qacaff";
	levelPwd[26] = "axcgxf";
	
	locked = false;
	dead = false;
	playing = false;
	level = 0;
	proceedToTheNextLevel = false;
	startTime = firstTime = -1;
	pwdMode = false;
}

Game::~Game()
{
}

void Game::loseLife()
{
	world->hero->visible = false;
	world->hero->x = world->hero->y = 100000;  
	dead = locked = true;
	//world->hero->body->resting = 100;
	diedY = -512;
	playSample(findSample(makeCode("loss")));
}

void Game::nextLevel()
{
	uint	time = (SDL_GetTicks() - startTime)/1000;
	level++;
	/*
	printf("took %i minutes and %i seconds to finish the level\n",
		time/60, time%60);
	*/
	if (level == 31)
	{
		playing = false;
		proceedToTheNextLevel = false;
		ending->activate();
		return;
	}

	startTime = -1;
	checkForPassword();
	restartLevel();
	proceedToTheNextLevel = false;
	randomTransition();
}

void Game::newGame(char *pass)
{
	level = 0;
	for (int i=0;i<30;i++)
		if (levelPwd[i] && !strcmp(pass, levelPwd[i]))
		{
			level = i;
			break;
		}
	proceedToTheNextLevel = false;
	startTime = firstTime = -1;
	pwdMode = false;
	checkForPassword();
	restartLevel();
}

void Game::checkForPassword()
{
	if (levelPwd[level])
		pwdMode = true;
}

void Game::restartLevel()
{
	char	buff[32];
	locked = false;
	dead = false;
	sprintf(buff, "data/worlds/world%i.w", level);
	world->loadWorld(buff);
	if (level == 30)
		world->bgndImage = 255;
	movingLeft = movingRight = jumping = wannaJump = false;
	jumpTicks = 0;
	world->hero->state->paused = true;
	if (startTime == -1)
		startTime = SDL_GetTicks();
	if (firstTime == -1)
		firstTime = startTime;
	if (level > 23)
	{
		world->world.resistance.x = 0.96f;
		world->world.resistance.y = 1.0f;
	}
	else
	{
		world->world.resistance.x = 0.9f;
		world->world.resistance.y = 0.99f;
	}
	playing = true;
	gtiShow = 0;
}

void Game::randomTransition()
{
	Transition 	*trans = createRandomTransition();
	trans->begin();
	draw();
	trans->play();
	delete trans;
	resetTimer();
}

void Game::enter()
{
	MosaicTransition	*trans = new MosaicTransition();
	trans->begin();
	draw();
	trans->play();
	delete trans;
	resetTimer();
}

void Game::leave()
{
}

void Game::timer()
{
	if (pwdMode)
		return;
	
	if (proceedToTheNextLevel)
	{
		nextLevel();
		return;
	}
	
	gtiShow++;
	
	if (dead && diedY < 176)
		diedY += 8;
	
	//#define SHOWTIMERONSCREEN
	#ifdef SHOWTIMERONSCREEN
	uint		time = (SDL_GetTicks() - startTime)/1000;
	uint		ttime = (SDL_GetTicks() - firstTime)/1000;
	static uint	lastTime = 0;
	if (time != lastTime)
	{
		char	bufi[64];
		sprintf(bufi,"level: %im:%is  total: %im:%is",
			time/60, time%60, ttime/60, ttime%60);
		SDL_WM_SetCaption(bufi, bufi);
		lastTime = time;
	}
	#endif
	
	if (!locked)
	{
		float	fv;
		
		if (movingLeft)
		{
			world->hero->state->paused = false;
			//world->hero->mc->mx = -3;
			
			if (world->hero->mc->vx > -3.0f)
			{
				fv = -3.0f - world->hero->mc->vx;
				world->hero->mc->applyForce(fv, 0);
			}
		}
		else if (movingRight)
		{
			world->hero->state->paused = false;
			//world->hero->mc->mx = 3;
			
			if (world->hero->mc->vx < 3.0f)
			{
				fv = 3.0f - world->hero->mc->vx;
				world->hero->mc->applyForce(fv, 0);
			}
		}
		else
		{
			world->hero->state->paused = true;
			//world->hero->mc->mx = 0;
		}
		
		if (wannaJump && world->hero->mc->onGround)
		{
			world->hero->mc->vy = 0;
			world->hero->mc->applyForce(0, -3);
			world->hero->mc->onGround = false;
			jumpTicks = 0;
			jumping = true;
			playSample(findSample(makeCode("jump")));
		}
		
		if (jumping)
		{
			jumpTicks++;
			if (jumpTicks > 13)
			{
				jumping = false;
				jumpTicks = 0;
			}
			
			if (world->hero->mc->vy > -5.0)
			{
				fv = -5.0f - world->hero->mc->vy;
				world->hero->mc->applyForce(0, fv);
			}
		}
		
		// for safety //
		/*
		if (world->hero->body->center.x < 0.0)
			world->hero->body->center.x = 0;
		if (world->hero->body->center.x > 639 - world->hero->w)
			world->hero->body->center.x = 639 - world->hero->w;
		if (world->hero->body->velocity.lengthSquared() > 256.0f)
		{
			SlashTDP::Vector	n = world->hero->body->
				velocity.normalized()*16.0f;
			world->hero->body->velocity = n;
		}*/
	}
	world->animate();
	
	for (Object *obj=world->firstObject;obj;obj=obj->next)
		obj->timer();

	Object	*next;
	for (Object *obj=world->firstObject;obj;obj=next)
	{
		next = obj->next;
		if (obj->deleteMe)
		{
			obj->deleteMe = false;
			world->removeObject(obj);
		}
		else if (obj->deleteMC)
		{
			obj->deleteMC = false;
			delete obj->mc;
			obj->mc = NULL;
		}
	}
}

void Game::draw()
{
	world->draw();
	if (!dead && world->icecream->visible && ((gtiShow/12)%4))
		gti->frame[0]->draw(485, 455, false);
	if (dead)
		died->frame[0]->draw(129, diedY, false);
	if (pwdMode)
	{
		int	x = 224;
		pwdBack->frame[0]->draw(129, 200, false);
		for (int i=0;i<6;i++,x+=32)
			letter[levelPwd[level][i]-'a']->frame[0]->draw(x, 270,
				false);
	}
}

void Game::handleEvent(SDL_Event &e)
{
	if (pwdMode)
	{
		if (e.type != SDL_KEYDOWN)
			return;
		if (e.key.keysym.sym == SDLK_SPACE ||
			e.key.keysym.sym == SDLK_RETURN ||
			e.key.keysym.sym == SDLK_ESCAPE)
				pwdMode = false;
		return;
	}
	
	switch (e.type)
	{
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
				case SDLK_SPACE:
					if (dead)
					{
						dead = locked = false;
						restartLevel();
						randomTransition();
					}
					break;
				case SDLK_F6:
					dead = locked = false;
					restartLevel();
					randomTransition();
					break;
				case SDLK_ESCAPE:
					menuScreen->pausedShow = 0;
					menuScreen->activate();
					break;
				case SDLK_UP:
					if (locked)
						break;
					wannaJump = true;
					break;
				case SDLK_LEFT:
					if (locked)
						break;
					movingLeft = true;
					world->hero->state->mirror = true;
					break;
				case SDLK_RIGHT:
					if (locked)
						break;
					movingRight = true;
					world->hero->state->mirror = false;
					break;
				case SDLK_F4:
					editor->activate();
					break;
				default:;
			}
			break;
		case SDL_KEYUP:
			switch (e.key.keysym.sym)
			{
				case SDLK_UP:
					if (locked)
						break;
					jumping = false;
					wannaJump = false;
					break;
				case SDLK_LEFT:
					if (locked)
						break;
					movingLeft = false;
					break;
				case SDLK_RIGHT:
					if (locked)
						break;
					movingRight = false;
					break;
				default:;
			}
			break;
	}
}

