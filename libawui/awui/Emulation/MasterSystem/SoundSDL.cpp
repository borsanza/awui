/**
 * awui/Emulation/MasterSystem/SoundSDL.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SoundSDL.h"
#include <assert.h>

using namespace awui::Emulation::MasterSystem;

#define SOUNDFREQ 44100
#define SOUNDSAMPLES 1024
#define BUFFERSIZE 2048
#define NUMBUFFERS 3
#define NUMCHANNELS 1

SoundSDL* SoundSDL::_instance = 0;

SoundSDL::SoundSDL() {
	_playing = NULL;
	writeBuf = 0;
	writePos = 0;
	readBuf = 0;

	bufs = new short[(long) BUFFERSIZE * NUMBUFFERS];
	if ( !bufs )
		return;
	
	semaphore = SDL_CreateSemaphore(NUMBUFFERS - 1);
	if (!semaphore)
		return; 

	SDL_AudioSpec wanted;

	memset(&wanted, 0, sizeof(wanted));
	wanted.freq = SOUNDFREQ;
	wanted.format = AUDIO_S16SYS;
	wanted.channels = NUMCHANNELS;
	wanted.samples = BUFFERSIZE / NUMCHANNELS;
	wanted.callback = SDLCallback;

	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec have;
	if (SDL_OpenAudio(&wanted, &have) < 0) {
        printf("Failed to open audio: %s", SDL_GetError());
	} else {
	    if (have.format != wanted.format) {
	        printf("We didn't get Float32 audio format.\n");
	    }

		SDL_PauseAudio(0);
	}

	soundOpen = true;
}

SoundSDL::~SoundSDL() {
	if (soundOpen) {
		soundOpen = false;
		SDL_PauseAudio( true );
		SDL_CloseAudio();
	}
	
	if (semaphore) {
		SDL_DestroySemaphore(semaphore);
		semaphore = NULL;
	}
	
	delete [] bufs;
	bufs = NULL;
}

SoundSDL* SoundSDL::Instance() {
	if (SoundSDL::_instance == 0)
		SoundSDL::_instance = new SoundSDL();

	return SoundSDL::_instance;
}

void SoundSDL::SDLCallback(void *data, uint8_t *stream, int len) {
	Instance()->FillBuffer(stream, len);
}

void SoundSDL::FillBuffer(uint8_t * out, int count) {
	if (SDL_SemValue(semaphore) < (NUMBUFFERS - 1)) {
		memcpy(out, GetBufPtr(readBuf), count);
		readBuf = (readBuf + 1) % NUMBUFFERS;
		SDL_SemPost(semaphore);
	} else {
		memset(out, 0, count);
	}
}

inline short* SoundSDL::GetBufPtr(int index)
{
	assert((unsigned) index < NUMBUFFERS);
	return bufs + (long) index * BUFFERSIZE;
}

void SoundSDL::Write(const short * in, int count) {
	while (count) {
		int n = BUFFERSIZE - writePos;

		if (n > count)
			n = count;
		
		memcpy(GetBufPtr(writeBuf) + writePos, in, n * sizeof (short));
		in += n;
		writePos += n;
		count -= n;
		
		if (writePos >= BUFFERSIZE) {
			writePos = 0;
			writeBuf = (writeBuf + 1) % NUMBUFFERS;
			SDL_SemWait(semaphore);
		}
	}
}