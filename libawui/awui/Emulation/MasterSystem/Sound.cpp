/*
 * awui/Emulation/MasterSystem/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>

using namespace awui::Emulation::MasterSystem;

Sound::Sound() {
	this->_type = 0;
	this->_noiseData = 0;
	this->_cpu = NULL;

	// Manera tonta de inicializar SoundSDL
	SoundSDL::Instance();

	this->_channel = 0;

	for (int i = 0; i <= 3; i++) {
		this->_channels[i]._volume = 0xF;
		this->_channels[i]._tone = 0x1;
		this->_channels[i]._fase = 0;
		this->_channels[i]._useModulation = false;
		for (int j = 0; j < SOUNDBUFFER; j++) {
			this->_channels[i]._buffer[j]._tone = 0;
			this->_channels[i]._buffer[j]._volume = 0xF;
			this->_channels[i]._buffer[j]._changeTone = false;
			this->_channels[i]._buffer[j]._changeVolume = false;
		}
	}
}

int Sound::GetPosBuffer(Motherboard * cpu) {
	SoundSDL * soundSDL = SoundSDL::Instance();
	double now = cpu->GetVirtualTime();
	now = now - soundSDL->GetInitTimeSound();
	float timeFrame = SOUNDSAMPLES / (SOUNDFREQ * 2.0f);
	int frame = (int(now / timeFrame)) % TOTALFRAMES;

	double pos = (double(SOUNDSAMPLES) * now) / timeFrame;
	pos = int(pos) % SOUNDSAMPLES;
	pos = pos + (frame * SOUNDSAMPLES);

//	printf("%d\n", (int)pos);
//	printf("------>>>> %d\n", this->_frame);

	return int(pos) % SOUNDBUFFER;
}

void Sound::WriteByte(Motherboard * cpu, uint8_t value) {
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
		SoundSDL * soundSDL = SoundSDL::Instance();

		int pos = this->GetPosBuffer(cpu);
		if (changeTone) {
			channel->_buffer[pos]._tone = (((channel->_tone != 0) || (this->_channel == 3)) ? channel->_tone : 1);
			channel->_buffer[pos]._changeTone = true;
			channel->_useModulation = false;
		}

		if (useModulation) {
			channel->_buffer[pos]._tone = channel->_register & 0xF;
			channel->_buffer[pos]._changeTone = true;
			channel->_buffer[pos]._changeVolume = true;
			channel->_buffer[pos]._volume = 31;
			channel->_useModulation = true;
		}

		if (changeVolume) {
			// 31 es un buen valor para no salirse del rango. 31 * 4 = 124/-124 (cabe en un int8)
			channel->_buffer[pos]._volume = (int8_t) (((15.0f - channel->_volume) / 15.0f) * 31.0f);
			channel->_buffer[pos]._changeVolume = true;
		}

		soundSDL->AddSound(this);
	}

	// printf("Channel: %d Volumen: %.2X\n", this->_channel, channel->_volume);
}
