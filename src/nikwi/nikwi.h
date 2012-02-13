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

#ifndef __NIKWI_H_INCLUDED__
#define __NIKWI_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL.h>
#ifdef main
#undef main
#endif

#ifdef WIN32
#define WIN32_MEAN_AND_LEAN
#define WIN32_LEAN_AND_MEAN
#define WIN32_MORE_LEAN
#define WIN32_MORE_MEAN
#define MORE_LEAN
#define MORE_MEAN
#include <windows.h>
#endif

#include <badcfg.h>
#include <uscript/uscript.h>
#include <slashtdp/slashtdp.h>
#include <slashfx.h>

#ifndef __linux__
typedef unsigned int	uint;
#endif
typedef char		*String;

#include "utils.h"
#include "nikio.h"
#include "gfx.h"
#include "sound.h"
#include "sprites.h"
#include "tiles.h"
#include "objects.h"
#include "motion.h"
#include "world.h"
#include "screens.h"
#include "editor.h"
#include "game.h"
#include "menu.h"
#include "imageScreen.h"
#include "ending.h"
#include "transitions.h"

extern bool	running;
extern bool	debugMode;

void resetTimer();

#endif

