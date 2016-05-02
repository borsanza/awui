/*
 * awui/Emulation/Spectrum/SoundSDL.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "SoundSDL.h"

#include <awui/DateTime.h>
#include <awui/Object.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Spectrum/Sound.h>

using namespace awui::Emulation::Spectrum;
using namespace awui::Collections;

extern void FillAudioSpectrumCB(void *udata, Uint8 *stream, int len);

SoundSDL* SoundSDL::_instance = 0;

SoundSDL::SoundSDL() {
	this->_playing = NULL;
	this->_frame = 0;

	SDL_Init(SDL_INIT_AUDIO);
	this->_wanted.freq = SOUNDFREQ;
    this->_wanted.format = SOUNDFORMAT == 1 ? AUDIO_S8 : AUDIO_S16SYS;
    this->_wanted.channels = 1;
    this->_wanted.samples = SOUNDSAMPLES;
    this->_wanted.callback = FillAudioSpectrumCB;
    this->_wanted.userdata = 0;
    this->_tone = 0;
	SDL_OpenAudio(&this->_wanted, NULL);
	this->_initTimeSound = awui::DateTime::GetTotalSeconds();
	SDL_PauseAudio(0);
}

SoundSDL* SoundSDL::Instance() {
	if (SoundSDL::_instance == 0)
		SoundSDL::_instance = new SoundSDL();

	return SoundSDL::_instance;
}

void FillAudioSpectrumCB(void *userdata, Uint8 *stream, int len) {
	SoundSDL::Instance()->FillAudio(stream, len);
}

void SoundSDL::FillAudio(Uint8 *stream, int len) {
	ArrayList copy;
	for (int i = 0; i < this->_arraySound.GetCount(); i++)
		copy.Add(this->_arraySound.Get(i));
	this->_arraySound.Clear();

	Sound * playing = this->_playing;
	for (int i = 0; i < copy.GetCount(); i++) {
		Sound * sound = (Sound *) copy.Get(i);
		if (playing != sound)
			this->FillAudioSDL(sound, 0, len);
	}

	if (playing)
		this->FillAudioSDL(playing, stream, len);
}

void SoundSDL::FillAudioSDL(Sound * sound, Uint8 *stream, int len) {
	int offset = this->_frame * SOUNDSIZEFRAME;

	for (int i = 0; i < len; i++) {
		int bufferPos = offset + i;

		Channel * channel = &sound->_channels;

		if (stream) {
			if (channel->_buffer[bufferPos]._signal != 0) {
				_tone = channel->_buffer[bufferPos]._signal;
			}

			stream[i] = _tone;
		}

		if (channel->_buffer[bufferPos]._signal != 0)
			channel->_buffer[bufferPos]._signal = 0;
	}

	this->_frame = (this->_frame + 1) % TOTALFRAMES;
}

void SoundSDL::AddSound(Sound * sound) {
	if (this->_arraySound.IndexOf(sound) == -1)
		this->_arraySound.Add(sound);
}
