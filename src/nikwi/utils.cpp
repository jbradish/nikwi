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
** Nikwi Engine - Utilities
*/

#include "nikwi.h"

void fatal(String msg)
{
	#ifdef WIN32
	MessageBox(NULL, msg, "Fatal error", MB_ICONERROR|MB_OK);
	#else
	fprintf(stderr, "Fatal error: %s", msg);
	#endif
	exit(1);
}

BadCFGNode *loadCFG(String file)
{
	BadCFGNode	*root;
	String		textData = getStringData(file);
	if (!textData)
		return NULL;
	root = parseBadCFGCode(textData);
	free(textData);

	return root;
}

uint makeCode(String code)
{
	return code[0]|(code[1] << 8)|(code[2] << 16)|(code[3] << 24);
}

bool rectOverRect(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2,
	int by2)
{
	return !(bx1 > ax2 || bx2 < ax1 || by1 > ay2 || by2 < ay1);
}

