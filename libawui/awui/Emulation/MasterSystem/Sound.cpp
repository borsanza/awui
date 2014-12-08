/*
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

using namespace awui::Emulation::MasterSystem;

#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/DateTime.h>
#include <math.h>
#include <stdio.h>

using namespace awui;

const double pi = 3.1415926535897932384626433832795029L;

extern void fill_audio(void *udata, Uint8 *stream, int len);

Sound* Sound::_instance = 0;

Sound::Sound() {
	this->_frame = 0;
//	this->_lastTime = this->_actualTime = cpu->GetRealTime();

	for (int i = 0; i <= 3; i++) {
		this->_channels[i]._volume = 0xF;
		this->_channels[i]._tone = 0x0;
		this->_channels[i]._fase = 0;
		for (int j = 0; j < SOUNDBUFFER; j++)
			this->_channels[i]._buffer[j] = 0;
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
#define LASTCHANNEL 3

void Sound::FillAudio(Uint8 *stream, int len) {
	int offset = this->_frame * SOUNDSIZEFRAME;
	for (int i = 0; i < len; i++) {
		int bufferPos = offset + i;

		int outputValue = 0;
		for (int j = FIRSTCHANNEL; j <= LASTCHANNEL; j++) {
			if (_channels[j]._buffer[bufferPos] == 0)
				continue;

			int v = 0;
			if (sin(_channels[j]._fase * pi * 2.0L / _channels[j]._buffer[bufferPos]) > 0)
				v = 42;
			else
				v = -42;

			outputValue += v;
			_channels[j]._fase++;
			_channels[j]._fase %= _channels[j]._buffer[bufferPos];
			_channels[j]._buffer[bufferPos] = 0;
		}

		if (outputValue > 127) outputValue = 127;        // and clip the result
		if (outputValue < -128) outputValue = -128;      // this seems a crude method, but works very well

		//printf("%d\n", outputValue);
        stream[i] = outputValue;
	}

	this->_frame = (this->_frame + 1) % TOTALFRAMES;

/*
	unsigned int bytesPerPeriod[4];
	for (int i = FIRSTCHANNEL; i <= LASTCHANNEL; i++) {
		if (this->_channels[i]._data != 0)
			bytesPerPeriod[i] =  SOUNDFREQ / this->_channels[i]._data;
		else
			bytesPerPeriod[i] = 0;
	}

	static unsigned int fase[4];

	for (int i = 0; i < len; i++) {
		int outputValue = 0;
		for (int j = FIRSTCHANNEL; j <= LASTCHANNEL; j++) {
			int channel = 0;

			if (this->_channels[j]._time == 0)
				continue;

			this->_channels[j]._time--;
			if (bytesPerPeriod[j] != 0)
				channel = int(40 * sin(fase[j] * 6.28 / bytesPerPeriod[j]));

			channel =  (channel * (15 - _channels[j]._volume)) / 15.0f;

			outputValue += channel;
		}

		if (outputValue > 127) outputValue = 127;        // and clip the result
		if (outputValue < -128) outputValue = -128;      // this seems a crude method, but works very well

        stream[i] = outputValue;

		for (int j = FIRSTCHANNEL; j <= LASTCHANNEL; j++) {
			fase[j]++;
			if (bytesPerPeriod[j] != 0)
				fase[j] %= bytesPerPeriod[j];

			if (fase[j] < 0)
				fase[j] = 0;
		}
	}
//*/
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
	Channel * channel = &(this->_channels[this->_channel]);
	bool mustSound = false;

	if ((value & 0x80) == 0) {
		//printf("%d %d%d%d%d%d%d\n", (value & 0x80 ? 1 : 0), (value & 0x20 ? 1 : 0), (value & 0x10 ? 1 : 0), (value & 0x8 ? 1 : 0), (value & 0x4 ? 1 : 0), (value & 0x2 ? 1 : 0), (value & 0x1 ? 1 : 0));
		channel->_tone = ((value & 0x3F) << 4) | (channel->_tone & 0x0F);

		if (this->_type == 0)
			mustSound = true;

		if (channel->_volume == 0xF)
			channel->_volume = 0;
	} else {
		//printf("%d %d%d %d %d%d%d%d\n", (value & 0x80 ? 1 : 0), (value & 0x40 ? 1 : 0), (value & 0x20 ? 1 : 0), (value & 0x10 ? 1 : 0), (value & 0x8 ? 1 : 0), (value & 0x4 ? 1 : 0), (value & 0x2 ? 1 : 0), (value & 0x1 ? 1 : 0));

		this->_channel = (value & 0x60) >> 5;
		this->_type = (value & 0x10) >> 4;
		channel = &(this->_channels[this->_channel]);

		channel->_tone = (channel->_tone & 0x3F0) | (value & 0x0F);
		if (this->_type == 1)
			channel->_volume = this->_data & 0xF;

		this->_data = value & 0x0F;


		if (this->_type == 0)
			mustSound = true;
		//printf("Channel: %d   Type: %d   Data: %.2X\n", this->_channel, this->_type, value & 0x0F);
	}


	if (!mustSound)
		return;

	if (channel->_volume == 0xF)
		return;

	if (channel->_tone != 0) {
		float speed = (cpu->GetVDP()->GetNTSC() ? 3579545.0f : 3546893.0f) / (16.0f * SOUNDFORMAT);
		float data = speed / channel->_tone;
		// printf("%f\n", data);

		unsigned int bytesPerPeriod = SOUNDFREQ / data;
		if (bytesPerPeriod != 0) {
			int pos = this->GetPosBuffer(cpu);
			for (int i = pos; i < (pos + 4096); i++)
				channel->_buffer[i % SOUNDBUFFER] = SOUNDFREQ / data;
		}
	}
}
