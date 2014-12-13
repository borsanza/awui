/*
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/DateTime.h>
#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include "SoundSDL.h"
#include <math.h>
#include <stdio.h>

using namespace awui;
using namespace awui::Emulation::MasterSystem;

Sound::Sound() {
	// Manera tonta de inicializar SoundSDL
	SoundSDL::Instance();

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
}

int Sound::GetPosBuffer(CPUInst * cpu) {
	SoundSDL * soundSDL = SoundSDL::Instance();
	double now = cpu->GetVirtualTime();
	now = now - soundSDL->GetInitTimeSound();
	float timeFrame = SOUNDSAMPLES / (SOUNDFREQ * 2.0f);
	int frame = ((int)floor(now / timeFrame)) % TOTALFRAMES;

	double pos = (double(SOUNDSIZEFRAME) * now) / timeFrame;
	pos = int(pos) % SOUNDSIZEFRAME;
	pos = pos + (frame * SOUNDSIZEFRAME);

//	printf("%d\n", (int)pos);
//	printf("------>>>> %d\n", this->_frame);

	return int(pos) % SOUNDBUFFER;
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
			channel->_buffer[pos]._tone = (((channel->_tone != 0) || (this->_channel == 3)) ? channel->_tone : 1);
			channel->_buffer[pos]._changeTone = true;
		}

		if (useModulation) {
// Hasta mejorarlo omito la Modulacion
/*
			channel->_buffer[pos]._tone = channel->_register & 0xF;
			channel->_buffer[pos]._changeTone = true;
			channel->_buffer[pos]._changeVolume = true;
			channel->_buffer[pos]._volume = 31;
*/
		}

		if (changeVolume) {
			// 31 es un buen valor para no salirse del rango. 31 * 4 = 124/-124 (cabe en un int8)
			channel->_buffer[pos]._volume = (int8_t) (((15.0f - channel->_volume) / 15.0f) * 31.0f);
			channel->_buffer[pos]._changeVolume = true;
		}
	}

	// printf("Channel: %d Volumen: %.2X\n", this->_channel, channel->_volume);
}
