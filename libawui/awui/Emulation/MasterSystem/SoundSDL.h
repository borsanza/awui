#pragma once

#include <stdint.h>
#include <SDL.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDSAMPLES 512
//#define SOUNDSAMPLES 1024
//#define SOUNDSAMPLES 2048

#define TOTALFRAMES 3
#define SOUNDBUFFER (SOUNDSAMPLES * TOTALFRAMES)

#include <awui/Collections/ArrayList.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Sound;

			class SoundSDL {
				private:
					int _frame;
					double _initTimeSound;
					SDL_AudioSpec _obtained;
					SDL_AudioDeviceID _audioDevice;

					awui::Collections::ArrayList _arraySound;
					Sound * _playing;
 					void ResetChannels(Sound * sound);
					static uint8_t m_disabledChannels;

					SoundSDL(const SoundSDL&) = delete;
					SoundSDL& operator=(const SoundSDL&) = delete;

				public:
					SoundSDL();
					~SoundSDL() = default;

					inline double GetInitTimeSound() { return this->_initTimeSound; }

					static SoundSDL& Instance();
					void FillAudio(Uint8 *stream, int len);
					void FillAudioSDL(Sound * sound, Uint8 *stream, int len);
					void AddSound(Sound * sound);

					inline void SetPlayingSound(Sound * sound) { this->_playing = sound; }

					static void ToggleChannel(int channel);
			};
		}
	}
}
