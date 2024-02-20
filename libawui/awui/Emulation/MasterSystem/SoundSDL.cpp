/**
 * awui/Emulation/MasterSystem/SoundSDL.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "SoundSDL.h"

#include <awui/Console.h>
#include <awui/DateTime.h>
#include <awui/Object.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>

using namespace awui::Emulation::MasterSystem;
using namespace awui::Collections;

extern void FillAudioMasterSystemCB(void *udata, Uint8 *stream, int len);

uint8_t	SoundSDL::m_disabledChannels = 0x00;

const char* GetAudioFormatName(SDL_AudioFormat format) {
	switch (format) {
		case AUDIO_U8:      return "AUDIO_U8";
		case AUDIO_S8:      return "AUDIO_S8";
		case AUDIO_U16LSB:  return "AUDIO_U16LSB";
		case AUDIO_S16LSB:  return "AUDIO_S16LSB";
		case AUDIO_U16MSB:  return "AUDIO_U16MSB";
		case AUDIO_S16MSB:  return "AUDIO_S16MSB";
		case AUDIO_S32LSB:  return "AUDIO_S32LSB";
		case AUDIO_S32MSB:  return "AUDIO_S32MSB";
		case AUDIO_F32LSB:  return "AUDIO_F32LSB";
		case AUDIO_F32MSB:  return "AUDIO_F32MSB";
		default:            return "Unknown Format";
	}
}

SoundSDL::SoundSDL() {
	this->_playing = NULL;
	this->_frame = 0;

	SDL_AudioSpec desired;
	SDL_zero(desired);
	desired.freq = SOUNDFREQ;
	desired.format = AUDIO_S16SYS;
	desired.channels = 2;
	desired.samples = SOUNDSAMPLES;
	desired.callback = FillAudioMasterSystemCB;
	desired.userdata = nullptr;

	this->_initTimeSound = awui::DateTime::GetTotalSeconds();

	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		SDL_Log("[ERROR] SDL_Init(SDL_INIT_AUDIO): %s", SDL_GetError());
		return;
	}

	this->_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desired, &this->_obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	if (this->_audioDevice == 0) {
		SDL_Log("[ERROR] SDL_OpenAudioDevice: %s", SDL_GetError());
	} else {
		if (this->_obtained.format != desired.format) {
			SDL_Log("[ERROR] Desired format: %s, but obtained format: %s.",
				GetAudioFormatName(desired.format), 
				GetAudioFormatName(this->_obtained.format));
		}

		SDL_PauseAudioDevice(this->_audioDevice, 0);
	}
}

SoundSDL& SoundSDL::Instance() {
    static SoundSDL instance;
    return instance;
}

void FillAudioMasterSystemCB(void *userdata, Uint8 *stream, int len) {
	SoundSDL::Instance().FillAudio(stream, len);
}

void SoundSDL::FillAudio(Uint8 *stream, int len) {
	ArrayList copy;
	for (int i = 0; i < this->_arraySound.GetCount(); i++)
		copy.Add(this->_arraySound.Get(i));
	this->_arraySound.Clear();

	Sound * playing = this->_playing;
	for (int i = 0; i < copy.GetCount(); i++) {
		Sound * sound = (Sound *) copy.Get(i);
		if (playing != sound)
			this->FillAudioSDL(sound, 0, len);
	}

	if (playing) {
		this->FillAudioSDL(playing, stream, len);
	} else {
		int numSamples = 0;
		switch (this->_obtained.format) {
			case AUDIO_S16SYS:
				numSamples = len / sizeof(Sint16);
				break;
			case AUDIO_F32LSB:
				numSamples = len / sizeof(float);
				break;
		}
		int offset = this->_frame * SOUNDSAMPLES;
		for (int i = 0; i < numSamples; i++) {		
			if (stream) {
				switch (this->_obtained.format) {
					case AUDIO_S16SYS: {
						auto* outStream = reinterpret_cast<Sint16*>(stream);
						outStream[i] = static_cast<Sint16>(0);
					}
					break;
					case AUDIO_F32LSB:
					{
						auto* outStream = reinterpret_cast<float*>(stream);
						outStream[i] = 0;
					}
					break;
				}
			}
		}
	}
}

void SoundSDL::FillAudioSDL(Sound * sound, Uint8 *stream, int len) {
	float speed = (SOUNDFREQ * 32.0f * 2.0f) / (sound->GetCPU()->GetVDP()->GetNTSC() ? 3579545.0f : 3546893.0f);

	int numSamples = 0;
	switch (this->_obtained.format) {
		case AUDIO_S16SYS:
			numSamples = len / sizeof(Sint16);
			break;
		case AUDIO_F32LSB:
			numSamples = len / sizeof(float);
			break;
	}

	const float preAmplificationFactor = 25.0f;

	int offset = this->_frame * SOUNDSAMPLES;
	for (int i = 0; i < numSamples; i++) {
		int bufferPos = offset + i;

		// FIXME: Hay desbordamiento de memoria y por eso pongo este if
		if (bufferPos >= SOUNDBUFFER) continue;

		int outputValue = 0;

		// Tone
		for (int j = 0; j <= 2; j++) {
			if (SoundSDL::m_disabledChannels & (1 << j)) {
				continue;
			}

			Channel * channel = &sound->m_channels[j];

			if (channel->_buffer[bufferPos]._changeTone) {
				channel->_buffer[bufferPos]._changeTone = false;
				channel->_last._tone = channel->_buffer[bufferPos]._tone;
				channel->_count = 4096;
			}

			if (channel->_buffer[bufferPos]._changeVolume) {
				channel->_buffer[bufferPos]._changeVolume = false;
				channel->_last._volume = channel->_buffer[bufferPos]._volume;
			}

			// Modulacion
			if (channel->_useModulation) {
				outputValue += (int8_t)((channel->_last._tone * 4) - 30);
				continue;
			}

			if (channel->_count <= 0)
				continue;

			channel->_count--;

			if (channel->_last._volume == 0)
				continue;

			float data = channel->_last._tone * speed;
			unsigned int bytesPerPeriod = data;
			if (bytesPerPeriod != 0) {
				if ((int((channel->_fase << 1) / data) % 2) == 0)
					outputValue += channel->_last._volume;
				else
					outputValue -= channel->_last._volume;

				channel->_fase++;
				channel->_fase %= bytesPerPeriod;
			}
		}

		// Noise
		if ((SoundSDL::m_disabledChannels & 0x08) == 0) {
			Channel * channel = &sound->m_channels[3];

			if (channel->_buffer[bufferPos]._changeTone) {
				channel->_buffer[bufferPos]._changeTone = false;
				channel->_last._tone = channel->_buffer[bufferPos]._tone;
				channel->_count = 0;
				sound->m_noiseData = 0x8000;
			}


			if (channel->_buffer[bufferPos]._changeVolume) {
				channel->_buffer[bufferPos]._changeVolume = false;
				channel->_last._volume = channel->_buffer[bufferPos]._volume;
			}

			if (channel->_last._volume != 0) {
				if (channel->_count == 0) {
					switch (channel->_last._tone & 0x3) {
						case 0: channel->_count = 0x10; break;
						case 1: channel->_count = 0x20; break;
						case 2: channel->_count = 0x40; break;
						case 3: channel->_count = sound->m_channels[2]._last._tone; break;
					}

					bool carry = false;
					if (channel->_last._tone & 0x4) {
						uint8_t v = sound->m_noiseData & 0x9;
						carry = (v == 0x1) || (v == 0x8);
					} else
						carry = sound->m_noiseData & 1;
					sound->m_noiseData = (sound->m_noiseData >> 1) | (carry ? 0x8000 : 0);
				}

				if (channel->_count > 0)
					channel->_count--;

				if (sound->m_noiseData & 1)
					outputValue += channel->_last._volume;
				else
					outputValue -= channel->_last._volume;
			}
		}


		if (stream) {
			outputValue = static_cast<int>(outputValue * preAmplificationFactor);

			switch (this->_obtained.format) {
				case AUDIO_S16SYS: {
					auto* outStream = reinterpret_cast<Sint16*>(stream);
					outStream[i] = static_cast<Sint16>(std::max(-32768, std::min(outputValue, 32767)));
				}
				break;
				case AUDIO_F32LSB:
				{
					auto* outStream = reinterpret_cast<float*>(stream);
					float sampleValue = outputValue / static_cast<float>(32768);
					sampleValue = std::max(-1.0f, std::min(sampleValue, 1.0f));
					outStream[i] = sampleValue;
				}
				break;
			}
		}
	}

	this->_frame = (this->_frame + 1) % TOTALFRAMES;
}

void SoundSDL::AddSound(Sound * sound) {
	if (this->_arraySound.IndexOf(sound) == -1)
		this->_arraySound.Add(sound);
}

void SoundSDL::ToggleChannel(int channel) {
	m_disabledChannels ^= 1 << channel;
//	Console::WriteLine("%x", m_disabledChannels);
}
