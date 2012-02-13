/*
 * libslashfx
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

#ifndef __SLASHFX_H_INCLUDED__
#define __SLASHFX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

/* both functions return 0 if something went wrong or 1 if everything went ok */

int ssfxLoadEffectFromMemory(char *ptr, short **wave, unsigned int *length);
int ssfxLoadEffectFromFile(char *fname, short **wave, unsigned int *length);

#ifdef __cplusplus
}
#endif

#endif
