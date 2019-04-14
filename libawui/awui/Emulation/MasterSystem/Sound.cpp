/*
 * awui/Emulation/MasterSystem/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>

using namespace awui::Emulation::MasterSystem;

#define SOUNDFREQ 44100
#define SOUNDSAMPLES 1024

Sound::Sound() {
	if (!buf.sample_rate(SOUNDFREQ))
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

const size_t out_size = 4096;
blip_sample_t out_buf[out_size];

void Sound::EndFrame(int length) {
	apu.end_frame(length);
	buf.end_frame(length);

	// Read some samples out of Blip_Buffer if there are enough to
	// fill our output buffer
	if (buf.samples_avail() >= out_size) {
		size_t count = buf.read_samples(out_buf, out_size);
		SoundSDL::Instance()->Write(out_buf, count);
	}
}
