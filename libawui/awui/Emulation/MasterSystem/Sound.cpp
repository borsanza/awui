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
	if (!buf.sample_rate(44100))
		printf("Out of memory\n");
	buf.clock_rate(3579545); // NTSC
	apu.output(&buf);

	this->_cpu = NULL;

	// Manera tonta de inicializar SoundSDL
	SoundSDL::Instance();
}

void Sound::WriteData(Motherboard *cpu, uint8_t value) {
	apu.write_data(cpu->GetVirtualTime(), value);
}

void Sound::WriteGGStereo(Motherboard *cpu, uint8_t value) {
	apu.write_ggstereo(cpu->GetVirtualTime(), value);
}
