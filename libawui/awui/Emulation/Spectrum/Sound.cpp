/*
 * awui/Emulation/Spectrum/Sound.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/Spectrum/Motherboard.h>
#include <awui/Emulation/Spectrum/SoundSDL.h>

using namespace awui::Emulation::Spectrum;

Sound::Sound() {
	// Manera tonta de inicializar SoundSDL
	SoundSDL::Instance();

	this->_channel = 0;
	for (int j = 0; j < SOUNDBUFFER; j++)
		this->_channels._buffer[j]._signal = 0;
}

int Sound::GetPosBuffer(Motherboard * cpu) {
	SoundSDL * soundSDL = SoundSDL::Instance();
	double now = cpu->GetVirtualTime();
	now = now - soundSDL->GetInitTimeSound();
	float timeFrame = SOUNDSAMPLES / (SOUNDFREQ * 2.0f);
	int frame = (int(now / timeFrame)) % TOTALFRAMES;

	double pos = (double(SOUNDSIZEFRAME) * now) / timeFrame;
	pos = int(pos) % SOUNDSIZEFRAME;
	pos = pos + (frame * SOUNDSIZEFRAME);

//	printf("%d\n", (int)pos);
//	printf("------>>>> %d\n", this->_frame);

	return int(pos) % SOUNDBUFFER;
}

void Sound::WriteByte(Motherboard * cpu, int16_t value) {
	int pos = this->GetPosBuffer(cpu);
	this->_channels._buffer[pos]._signal = value;
	SoundSDL::Instance()->AddSound(this);
}

void Sound::WriteSound(Motherboard * cpu, int value) {
    int signal = (value & 0x10) ? +85 : -85;
    signal += (value & 0x08) ? +42 : -42;

	this->WriteByte(cpu, signal);
}
