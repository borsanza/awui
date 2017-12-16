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
	this->_cpu = NULL;

	for (int j = 0; j < SOUNDBUFFER; j++)
		this->_buffer[j] = 0;

	// Manera tonta de inicializar SoundSDL
	SoundSDL::Instance();
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

	return int(pos) % SOUNDBUFFER;
}

void Sound::WriteByte(Motherboard * cpu, int16_t value) {
	int pos = this->GetPosBuffer(cpu);
	this->_buffer[pos] = value;
	SoundSDL::Instance()->AddSound(this);
}

void Sound::WriteSound(Motherboard * cpu, int value) {
	int signal = ((value >> 4) & 0x01) ? +85 : -85;
	signal    += ((value >> 3) & 0x01) ? +42 : -42;

	this->WriteByte(cpu, signal);
}
