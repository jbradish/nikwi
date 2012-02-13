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
** Nikwi Engine - Sound
*/

#include "nikwi.h"

bool	noSound = false;

static Sample		*sample = NULL;
static SampleQueueEntry	*first = NULL, *last = NULL;
static int		samplesToPlay = 0;
static bool		soundIsShuttingDown = false;

static void removeSampleEntry(SampleQueueEntry *e)
{
	samplesToPlay--;
	if (e->prev)
		e->prev->next = e->next;
	else
		first = e->next;
	if (e->next)
		e->next->prev = e->prev;
	else
		last = e->prev;
	
	delete e;
}

static void sdlCallback(void*, Uint8 *stream, int len)
{
	short			*buff = (short*)stream;
	SampleQueueEntry	*next;
	
	len /= 2;
	if (!first || soundIsShuttingDown)
	{
		memset(stream, 0, len);
		return;
	}
	
	for (int i=0;i<len;i++)
	{
		int	sample = 0;
		
		for (SampleQueueEntry *e=first;e;e = next)
		{
			next = e->next;
			sample += e->sample->wave[e->pos++];
			 
			if (e->pos >= e->sample->length)
				removeSampleEntry(e);
		}
		
		if (sample > 32760)
			sample = 32760;
		else if (sample < -32760)
			sample = -32760;
		
		
		buff[i] = sample;
	}
}

void initSound()
{
	BadCFGNode	*samples = loadCFG("data/samples.txt");
	if (!samples)
		fatal("Cannot load audio samples configuration file");
	
	for (BadCFGNode *smp=samples->child;smp;smp=smp->next)
	{
		String	file;
		uint	code = makeCode(getBadCFGValue(smp, "code", "????"));
		
		file = (String)malloc(strlen(smp->value) + 17);
		sprintf(file, "data/audio/%s.ssfx", smp->value);
		
		uint	dataLen;
		char	*ssfx = (char*)getData(file, dataLen);
		free(file);
		
		if (!ssfx)
			continue;
		
		Sample	*sam = new Sample;
		sam->code = code;
		if (!ssfxLoadEffectFromMemory(ssfx, &sam->wave, &sam->length))
		{
			delete sam;
			continue;
		}
		free(ssfx);
		
		sam->next = sample;
		sample = sam;
	}
	
	destroyBadCFGNode(samples);
	
	
	SDL_AudioSpec	spec;
        spec.freq = 22050;
        spec.channels = 1;
        spec.format = AUDIO_S16;
        spec.samples = 1024;
        spec.callback = sdlCallback;
        if (SDL_OpenAudio(&spec, NULL) < 0)
                noSound = true;
		//fprintf(stderr, "SNDError: %s\n", SDL_GetError());
	else
		SDL_PauseAudio(0);
	
	playSample(findSample(makeCode("lgbl")));
}

void shutdownSound()
{
	Sample	*next;
	SDL_LockAudio();
	soundIsShuttingDown = true;
	while (sample)
	{
		next = sample->next;
		free(sample->wave);
		delete sample;
		sample = next;
	}
	SDL_UnlockAudio();
}

Sample *findSample(uint code)
{
	for (Sample *smp=sample;smp;smp = smp->next)
		if (smp->code == code)
			return smp;
	return NULL;
}

void playSample(Sample *sample)
{
	SampleQueueEntry	*e;
	
	if (!sample || samplesToPlay >= 48 || noSound)
		return;
	
	for (e=first;e;e = e->next)
		if (e->sample == sample && e->pos < 128)
			return;
	
	SDL_LockAudio();
	e = new SampleQueueEntry;
	e->sample = sample;
	e->pos = 0;
	e->prev = last;
	e->next = NULL;
	if (first)
		last->next = e;
	else
		first = e;
	last = e;

	samplesToPlay++;

	SDL_UnlockAudio();
}

