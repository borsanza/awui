#ifndef _AWUI_EMULATION_SPECTRUM_SOUNDSDL_H
#define _AWUI_EMULATION_SPECTRUM_SOUNDSDL_H

#include <stdint.h>
#include <SDL.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDFORMAT 1
#define SOUNDSAMPLES 1024
//#define SOUNDSAMPLES 2048

#define TOTALFRAMES 3
#define SOUNDSIZEFRAME ((SOUNDSAMPLES * SOUNDFORMAT) / 2)
#define SOUNDBUFFER (SOUNDSIZEFRAME * TOTALFRAMES)

#include <awui/Collections/ArrayList.h>

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			class CPUInst;
			class Sound;

			class SoundSDL {
				private:
					static SoundSDL* _instance;
					int _frame;
					double _initTimeSound;
					SDL_AudioSpec _wanted;

					awui::Collections::ArrayList _arraySound;
					Sound * _playing;

				public:
					SoundSDL();

					inline double GetInitTimeSound() { return this->_initTimeSound; }

					static SoundSDL* Instance();
					void FillAudio(Uint8 *stream, int len);
					void FillAudioSDL(Sound * sound, Uint8 *stream, int len);
					void AddSound(Sound * sound);

					inline void SetPlayingSound(Sound * sound) { this->_playing = sound; }
			};
		}
	}
}

#endif
