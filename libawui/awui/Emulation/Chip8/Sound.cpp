/*
 * awui/Emulation/Chip8/Sound.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Sound.h"

using namespace awui::Emulation::Chip8;

#include <fcntl.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <unistd.h>

Sound::Sound() {
	this->_playing = true;
	this->_console_fd = open("/dev/console", O_WRONLY);
	this->Stop();
}

Sound::~Sound() {
	this->Stop();
	close(this->_console_fd);
}

void Sound::Play() {
	if (!this->_playing) {
		ioctl(this->_console_fd, KIOCSOUND, 1193180/440);
		this->_playing = true;
	}
}

void Sound::Stop() {
	if (this->_playing) {
		ioctl(this->_console_fd, KIOCSOUND, 0);
		this->_playing = false;
	}
}