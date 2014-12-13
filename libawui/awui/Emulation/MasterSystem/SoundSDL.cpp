/*
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SoundSDL.h"

#include <awui/DateTime.h>
#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <math.h>
#include <stdio.h>

using namespace awui;
using namespace awui::Emulation::MasterSystem;

extern void FillAudioCB(void *udata, Uint8 *stream, int len);

SoundSDL* SoundSDL::_instance = 0;

SoundSDL::SoundSDL() {
	this->_cpu = NULL;
	this->_frame = 0;

	SDL_Init(SDL_INIT_AUDIO);
	this->_wanted.freq = SOUNDFREQ;
    this->_wanted.format = SOUNDFORMAT == 1 ? AUDIO_S8 : AUDIO_S16SYS;
    this->_wanted.channels = 1;
    this->_wanted.samples = SOUNDSAMPLES;
    this->_wanted.callback = FillAudioCB;
    this->_wanted.userdata = 0;
	SDL_OpenAudio(&this->_wanted, NULL);
	this->_initTimeSound = DateTime::GetTotalSeconds();
	SDL_PauseAudio(0);
}

SoundSDL* SoundSDL::Instance() {
	if (SoundSDL::_instance == 0)
		SoundSDL::_instance = new SoundSDL();

	return SoundSDL::_instance;
}

void FillAudioCB(void *userdata, Uint8 *stream, int len) {
	SoundSDL * sound = SoundSDL::Instance();
	sound->FillAudio(stream, len);
    }

void SoundSDL::FillAudio(Uint8 *stream, int len) {
	if (this->_cpu == NULL)
		return;

	Sound * sound = this->_cpu->GetSound();

	float speed = (SOUNDFREQ * 32.0f * SOUNDFORMAT) / (this->_cpu->GetVDP()->GetNTSC() ? 3579545.0f : 3546893.0f);

	int offset = this->_frame * SOUNDSIZEFRAME;
	for (int i = 0; i < len; i++) {
		int bufferPos = offset + i;

		int outputValue = 0;

		// Tone
		for (int j = 0; j <= 2; j++) {
			Channel * channel = &sound->_channels[j];

			if (channel->_buffer[bufferPos]._changeTone) {
				channel->_buffer[bufferPos]._changeTone = false;
				channel->_last._tone = channel->_buffer[bufferPos]._tone;
				channel->_count = 4096;
			}

			if (channel->_buffer[bufferPos]._changeVolume) {
				channel->_buffer[bufferPos]._changeVolume = false;
				channel->_last._volume = channel->_buffer[bufferPos]._volume;
			}

			if (channel->_count == 0)
				continue;

			channel->_count--;

			if (channel->_last._volume == 0)
				continue;

			// Modulacion
			// outputValue += (int8_t)((channel->_last._tone * 4) - 30);
			// continue;

			float data = channel->_last._tone * speed;
			unsigned int bytesPerPeriod = data;
			if (bytesPerPeriod != 0) {
				if ((int((channel->_fase << 1) / data) % 2) == 0)
					outputValue += channel->_last._volume;
				else
					outputValue -= channel->_last._volume;

				channel->_fase++;
				channel->_fase %= bytesPerPeriod;
			}
		}

		// Noise
		{
			Channel * channel = &sound->_channels[3];

			if (channel->_buffer[bufferPos]._changeTone) {
				channel->_buffer[bufferPos]._changeTone = false;
				channel->_last._tone = channel->_buffer[bufferPos]._tone;
				channel->_count = 0;
				sound->_noiseData = 0x8000;
			}

			if (channel->_buffer[bufferPos]._changeVolume) {
				channel->_buffer[bufferPos]._changeVolume = false;
				channel->_last._volume = channel->_buffer[bufferPos]._volume;
			}

			if (channel->_last._volume != 0) {
				if (channel->_count == 0) {
					switch (channel->_last._tone & 0x3) {
						case 0: channel->_count = 0x10; break;
						case 1: channel->_count = 0x20; break;
						case 2: channel->_count = 0x40; break;
						case 3: channel->_count = sound->_channels[2]._last._tone; break;
					}

					bool carry = false;
					if (channel->_last._tone & 0x4) {
						uint8_t v = sound->_noiseData & 0x9;
						carry = (v == 0x1) || (v == 0x8);
					} else
						carry = sound->_noiseData & 1;
					sound->_noiseData = (sound->_noiseData >> 1) | (carry ? 0x8000 : 0);
				}

				channel->_count--;

				if (sound->_noiseData & 1)
					outputValue += channel->_last._volume;
				else
					outputValue -= channel->_last._volume;
			}
		}

        stream[i] = outputValue;
	}

	this->_frame = (this->_frame + 1) % TOTALFRAMES;
}
