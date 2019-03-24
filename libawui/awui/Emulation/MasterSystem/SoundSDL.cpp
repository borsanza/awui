/**
 * awui/Emulation/MasterSystem/SoundSDL.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SoundSDL.h"

#include <awui/DateTime.h>
#include <awui/Object.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>

using namespace awui::Emulation::MasterSystem;
using namespace awui::Collections;

extern void FillAudioMasterSystemCB(void *udata, Uint8 *stream, int len);

SoundSDL* SoundSDL::_instance = 0;

SoundSDL::SoundSDL() {
	this->_playing = NULL;
	this->_frame = 0;

	memset(&this->_wanted, 0, sizeof(this->_wanted));
	this->_wanted.freq = SOUNDFREQ;
	this->_wanted.format = AUDIO_S16SYS;
	this->_wanted.channels = 1;
	this->_wanted.samples = SOUNDSAMPLES;
	this->_wanted.callback = FillAudioMasterSystemCB;

	this->_initTimeSound = awui::DateTime::GetTotalSeconds();

	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec have;
	if (SDL_OpenAudio(&this->_wanted, &have) < 0) {
        printf("Failed to open audio: %s", SDL_GetError());
	} else {
	    if (have.format != this->_wanted.format) {
	        printf("We didn't get Float32 audio format.\n");
	    }

		SDL_PauseAudio(0);
	}
}

SoundSDL* SoundSDL::Instance() {
	if (SoundSDL::_instance == 0)
		SoundSDL::_instance = new SoundSDL();

	return SoundSDL::_instance;
}

void FillAudioMasterSystemCB(void *userdata, Uint8 *stream, int len) {
	SoundSDL::Instance()->FillAudio(stream, len);
}

void SoundSDL::FillAudio(Uint8 *stream, int len) {

}
