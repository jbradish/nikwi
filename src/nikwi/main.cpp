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
** Nikwi - a game by Kostas Michalopoulos
*/

#include "nikwi.h"

bool	running = true;
bool	debugMode = false;
int	debugCounter = 0;

static void timedTasks()
{
	activeScreen->timer();
}

static uint	extra = 0;
static uint	lastTicks = 0, ticks = 0;

static void draw()
{
	ticks = SDL_GetTicks();
	if (ticks - lastTicks > 20)
	{
		uint	diff = ticks - lastTicks + extra;
		uint	times = diff/20;
		extra = diff;
		for (uint i=0;i<times;i++,extra -= 20)
			timedTasks();
		lastTicks = ticks;
	}
	
	activeScreen->draw();
	
	updateSystemScreen();
}

void resetTimer()
{
	ticks = lastTicks = SDL_GetTicks();
}

static void translateJoyMotionToKey(SDL_Event &e)
{
	static int	prevXAxis = 0;
	static int	prevYAxis = 0;
	
	if (e.jaxis.axis == 0)	// x-axis
	{
		if (e.jaxis.value > 32000)
		{
			prevXAxis = e.jaxis.value;
			e.type = SDL_KEYDOWN;
			e.key.keysym.sym = SDLK_RIGHT;
		}
		else if (e.jaxis.value < -32000)
		{
			prevXAxis = e.jaxis.value;
			e.type = SDL_KEYDOWN;
			e.key.keysym.sym = SDLK_LEFT;
		}
		else
		{
			e.type = SDL_KEYUP;
			if (prevXAxis > 32000)
				e.key.keysym.sym = SDLK_RIGHT;
			else
				e.key.keysym.sym = SDLK_LEFT;
		}
	}
	if (e.jaxis.axis == 1)	// y-axis
	{
		if (e.jaxis.value > 32000)
		{
			prevYAxis = e.jaxis.value;
			e.type = SDL_KEYDOWN;
			e.key.keysym.sym = SDLK_DOWN;
		}
		else if (e.jaxis.value < -32000)
		{
			if (game == activeScreen && !game->dead)
				return;
			prevYAxis = e.jaxis.value;
			e.type = SDL_KEYDOWN;
			e.key.keysym.sym = SDLK_UP;
		}
		else
		{
			e.type = SDL_KEYUP;
			if (prevYAxis > 32000)
				e.key.keysym.sym = SDLK_DOWN;
			else
				e.key.keysym.sym = SDLK_UP;
		}
	}
}

static void mainLoop()
{
	SDL_Event	e;
	
	while (running)
	{
		usRunScripts();
		draw();
		
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_JOYAXISMOTION:
					translateJoyMotionToKey(e);
					break;
				case SDL_JOYBUTTONDOWN:
					if (e.jbutton.button == 0)
					{
						e.type = SDL_KEYDOWN;
						e.key.keysym.sym =
							(activeScreen == game &&
								!game->dead)?
								SDLK_UP:
								SDLK_SPACE;
					}
					break;
				case SDL_JOYBUTTONUP:
					if (e.jbutton.button == 0)
					{
						e.type = SDL_KEYUP;
						e.key.keysym.sym =
							(activeScreen == game &&
								!game->dead)?
								SDLK_UP:
								SDLK_SPACE;
					}
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
						case SDLK_s:
							if (debugCounter < 2)
								debugCounter++;
							break;
						case SDLK_d:
							if (debugCounter == 2)
								debugCounter++;
							break;
						case SDLK_b:
							if (debugCounter == 3)
								debugCounter++;
							break;
						case SDLK_g:
							if (debugCounter == 4)
							{
								debugMode =
								     !debugMode;
								debugCounter =0;
								if (!debugMode)
					    SDL_WM_SetCaption("Nikwi", "Nikwi");
							}
							break;
						case SDLK_F10:
							running = false;
						default:;
					}
					break;					
				case SDL_QUIT:
					running = false;
					break;
			}
			activeScreen->handleEvent(e);
		}
	}
}

#ifdef __APPLE__
extern "C" int SDL_main(int argn, char** argv)
#else
int main(int argn, char *argv[])
#endif
{
	bool	editMode = false;
	uint	startLevel = 0;
	
	for (int i=0;i<argn;i++)
	{
		if (!strcmp(argv[i], "--enable-editor"))
			editMode = true;
		else if (!strcmp(argv[i], "--start-level"))
			startLevel = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--last-level"))
			startLevel = 30;
		else if (!strcmp(argv[i], "--fullscreen"))
			fullscreen = true;
		else if (!strcmp(argv[i], "--nosound"))
			noSound = true;
	}
	
	if (!initGfx("Nikwi"))
	{
		#ifdef WIN32
		MessageBox(NULL, "Cannot initialize graphics mode 640x480 in "
			"32 bits per pixel mode. Try to change the color depth "
			"if your screen from Desktop properties.", "Error",
			MB_ICONERROR|MB_OK);
		#else
		fprintf(stderr, "Cannot initialize graphics mode 640x480 in "
			"32 bits per pixel mode. Are you under X Windows?");
		#endif
		return 1;
	}
	
	usInitialize();
	
	initNikIO();
	initSound();
	initSprites();
	initTiles();
	initObjects();
	initScreens();

	world = new World;
	editor = new Editor;
	game = new Game;
	game->level = startLevel;
	editor->aWorld = startLevel;
	
	menuScreen = new MenuScreen;
	imageScreen = new ImageScreen;
	ending = new EndingScreen;
	
	SDL_Delay(500);
	
	resetTimer();
	if (editMode)
		editor->activate();
	else
	{
		if (startLevel)
		{
			game->newGame();
			game->level = startLevel;
			game->restartLevel();
			game->activate();
		}
		else
			imageScreen->showImage(menuScreen,
				"data/objects/slashstone.ut", 160);
	}
	mainLoop();
	activeScreen->leave();
	
	delete ending;
	delete imageScreen;
	delete menuScreen;
	delete game;
	delete editor;
	delete world;
	
	shutdownScreens();
	shutdownObjects();
	shutdownTiles();
	shutdownSprites();
	shutdownSound();
	shutdownNikIO();
	
	usShutdown();
	
	shutdownGfx();
	
	return 0;
}

