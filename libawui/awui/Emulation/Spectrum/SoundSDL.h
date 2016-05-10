#ifndef _AWUI_EMULATION_SPECTRUM_SOUNDSDL_H
#define _AWUI_EMULATION_SPECTRUM_SOUNDSDL_H

#include <stdint.h>
#include <SDL2/SDL.h>

#include <awui/Collections/ArrayList.h>

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			class Sound;

			class SoundSDL {
				private:
					static SoundSDL* _instance;
					int _frame;
					double _initTimeSound;
					SDL_AudioSpec _wanted;

					awui::Collections::ArrayList _arraySound;
					Sound * _playing;
					int _tone;

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
