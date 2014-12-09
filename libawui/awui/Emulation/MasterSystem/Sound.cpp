/*
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/DateTime.h>
#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <math.h>
#include <stdio.h>

using namespace awui;
using namespace awui::Emulation::MasterSystem;

extern void fill_audio(void *udata, Uint8 *stream, int len);

Sound* Sound::_instance = 0;

Sound::Sound() {
	this->_frame = 0;
	this->_channel = 0;

	for (int i = 0; i <= 3; i++) {
		this->_channels[i]._volume = 0xF;
		this->_channels[i]._tone = 0x1;
		this->_channels[i]._fase = 0;
		for (int j = 0; j < SOUNDBUFFER; j++) {
			this->_channels[i]._buffer[j]._tone = 0;
			this->_channels[i]._buffer[j]._volume = 0xF;
			this->_channels[i]._buffer[j]._changeTone = false;
			this->_channels[i]._buffer[j]._changeVolume = false;
		}
	}

	SDL_Init(SDL_INIT_AUDIO);
	this->_wanted.freq = SOUNDFREQ;
    this->_wanted.format = SOUNDFORMAT == 1 ? AUDIO_S8 : AUDIO_S16SYS;
    this->_wanted.channels = 1;
    this->_wanted.samples = SOUNDSAMPLES;
    this->_wanted.callback = fill_audio;
    this->_wanted.userdata = 0;
	SDL_OpenAudio(&this->_wanted, NULL);
	this->_initTimeSound = DateTime::GetTotalSeconds();
	SDL_PauseAudio(0);
}

Sound::~Sound() {
}

Sound* Sound::Instance() {
	if (Sound::_instance == 0)
		Sound::_instance = new Sound;

	return Sound::_instance;
}

#define FIRSTCHANNEL 0
#define LASTCHANNEL 2

void Sound::FillAudio(Uint8 *stream, int len) {
	if (this->_cpu == NULL)
		return;

	float speed = (this->_cpu->GetVDP()->GetNTSC() ? 3579545.0f : 3546893.0f) / (32.0f * SOUNDFORMAT); //

	int offset = this->_frame * SOUNDSIZEFRAME;
	for (int i = 0; i < len; i++) {
		int bufferPos = offset + i;

		int outputValue = 0;
		for (int j = FIRSTCHANNEL; j <= LASTCHANNEL; j++) {
			Channel * channel = &this->_channels[j];

			if (channel->_buffer[bufferPos]._changeTone) {
				channel->_buffer[bufferPos]._changeTone = false;
				channel->_last._tone = channel->_buffer[bufferPos]._tone;
				//channel->_lastAmplitude = 0;
				channel->_count = 4096;
			}

			if (channel->_buffer[bufferPos]._changeVolume) {
				channel->_buffer[bufferPos]._changeVolume = false;
				channel->_last._volume = channel->_buffer[bufferPos]._volume;
			}

			if (channel->_last._volume == 0xF)
				continue;

			if (channel->_count == 0)
				continue;

			channel->_count--;

			if (channel->_last._tone == 0)
				continue;

			float data = speed / channel->_last._tone;
			if (data == 0)
				continue;

			data = SOUNDFREQ / data;
			unsigned int bytesPerPeriod = data;
			if (bytesPerPeriod == 0)
				continue;

			// bool up = sin(channel->_fase * pi * 2.0L / data) > 0;
			bool up = (int(floor((channel->_fase << 1) / data)) % 2) == 0;
			float v = up ? 32 : -32;
/*
			if (up) {
				if (channel->_lastAmplitude <= 0) channel->_lastAmplitude = 32;
				if (channel->_lastAmplitude > 0)  channel->_lastAmplitude -= 0.3f;
				else channel->_lastAmplitude = 0;
			} else {
				if (channel->_lastAmplitude >= 0) channel->_lastAmplitude = -32;
				if (channel->_lastAmplitude < 0)  channel->_lastAmplitude += 0.3f;
				else channel->_lastAmplitude = 0;
			}
			v = channel->_lastAmplitude;
*/

			float amplitude =  (v * (15 - channel->_last._volume)) / 15.0f;

			outputValue += amplitude;
			channel->_fase++;
			channel->_fase %= bytesPerPeriod;
		}

		if (outputValue > 127) outputValue = 127;        // and clip the result
		if (outputValue < -128) outputValue = -128;      // this seems a crude method, but works very well

		// printf("%d\n", outputValue);
        stream[i] = outputValue;
	}

	this->_frame = (this->_frame + 1) % TOTALFRAMES;

//	printf("<<<------ %d\n", this->_frame);
}

void fill_audio(void *userdata, Uint8 *stream, int len) {
	Sound * sound = Sound::Instance();
	sound->FillAudio(stream, len);
    }

int Sound::GetPosBuffer(CPUInst * cpu) {
	double now = cpu->GetVirtualTime();
	now = now - _initTimeSound;
	float timeFrame = SOUNDSAMPLES / (SOUNDFREQ * 2.0f);
	int frame = ((int)floor(now / timeFrame)) % TOTALFRAMES;

	double pos = (double(SOUNDSIZEFRAME) * now) / timeFrame;
	pos = int(pos) % SOUNDSIZEFRAME;
	pos = pos + (frame * SOUNDSIZEFRAME);

//	printf("%d\n", (int)pos);
//	printf("------>>>> %d\n", this->_frame);

	return pos;
}

void Sound::WriteByte(CPUInst * cpu, uint8_t value) {
	bool changeTone = false;
	bool changeVolume = false;
	bool useModulation = false;

	Channel * channel = &(this->_channels[this->_channel]);

	if ((value & 0x80) == 0) {
		// printf("%d %d%d%d%d%d%d\n", (value & 0x80 ? 1 : 0), (value & 0x20 ? 1 : 0), (value & 0x10 ? 1 : 0), (value & 0x8 ? 1 : 0), (value & 0x4 ? 1 : 0), (value & 0x2 ? 1 : 0), (value & 0x1 ? 1 : 0));
		channel->_register = ((value & 0x3F) << 4) | (channel->_register & 0x0F);

		if (this->_type == 0) {
			changeTone = true;
			channel->_tone = channel->_register;
		} else {
			changeVolume = true;
			channel->_volume = value & 0xF;
		}
	} else {
		// printf("%d %d%d %d %d%d%d%d\n", (value & 0x80 ? 1 : 0), (value & 0x40 ? 1 : 0), (value & 0x20 ? 1 : 0), (value & 0x10 ? 1 : 0), (value & 0x8 ? 1 : 0), (value & 0x4 ? 1 : 0), (value & 0x2 ? 1 : 0), (value & 0x1 ? 1 : 0));

		this->_channel = (value & 0x60) >> 5;
		this->_type = (value & 0x10) >> 4;

		channel = &(this->_channels[this->_channel]);
		channel->_register = (channel->_register & 0x3F0) | (value & 0x0F);
		if (this->_type == 1) {
			if (channel->_tone == 0) {
				useModulation = true;
			} else {
				channel->_volume = value & 0xF;
				changeVolume = true;
			}
		} else {
			channel->_tone = channel->_register;
			changeTone = true;
		}
		// printf("Channel: %d   Type: %d   Data: %.2X\n", this->_channel, this->_type, value & 0x0F);
	}

	if (changeTone || changeVolume || useModulation) {
		int pos = this->GetPosBuffer(cpu);
		if (changeTone) {
			channel->_buffer[pos % SOUNDBUFFER]._tone = ((channel->_tone != 0) ? channel->_tone : 1);
			channel->_buffer[pos % SOUNDBUFFER]._changeTone = true;
		}

		if (useModulation) {
// Hasta mejorarlo omito la Modulacion
/*
			channel->_buffer[pos % SOUNDBUFFER]._tone = channel->_register;
			channel->_buffer[pos % SOUNDBUFFER]._changeTone = true;
			channel->_buffer[pos % SOUNDBUFFER]._changeVolume = true;
			channel->_buffer[pos % SOUNDBUFFER]._volume = 0;
*/
		}

		if (changeVolume) {
			channel->_buffer[pos % SOUNDBUFFER]._volume = channel->_volume;
			channel->_buffer[pos % SOUNDBUFFER]._changeVolume = true;
		}
	}

	// printf("Channel: %d Volumen: %.2X\n", this->_channel, channel->_volume);
}
