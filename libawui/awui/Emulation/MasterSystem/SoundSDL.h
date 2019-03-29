#pragma once

#include <stdint.h>
#include <SDL.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Sound;

			class SoundSDL {
				private:
					static SoundSDL* _instance;
					Sound * _playing;

					short * volatile bufs;
					SDL_sem* volatile semaphore;
					int volatile readBuf;
					bool soundOpen = false;
					int writePos;
					int writeBuf;

					static void SDLCallback(void *data, uint8_t *stream, int length);
					virtual void FillBuffer(uint8_t * stream, int length);
					short * GetBufPtr(int index);

				public:
					SoundSDL();
					~SoundSDL();

					static SoundSDL* Instance();

					inline void SetPlayingSound(Sound * sound) { this->_playing = sound; }
					void Write(const short * in, int count);
			};
		}
	}
}
