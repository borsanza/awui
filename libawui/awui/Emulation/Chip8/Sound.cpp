/**
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

using namespace awui::Emulation::Chip8;

#include <fcntl.h>
#include <unistd.h>

Sound::Sound() {
	m_playing = true;
	m_consoleFd = open("/dev/console", O_WRONLY);
	Stop();
}

Sound::~Sound() {
	Stop();
	close(m_consoleFd);
}

void Sound::Play() {
	if (!m_playing) {
		//		ioctl(m_consoleFd, KIOCSOUND, 1193180/440);
		m_playing = true;
	}
}

void Sound::Stop() {
	if (m_playing) {
		//		ioctl(m_consoleFd, KIOCSOUND, 0);
		m_playing = false;
	}
}
