#ifndef _AWUI_EMULATION_MASTERSYSTEM_SOUNDSDL_H
#define _AWUI_EMULATION_MASTERSYSTEM_SOUNDSDL_H

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

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class CPUInst;

			class SoundSDL {
				private:
					static SoundSDL* _instance;
					int _frame;
					double _initTimeSound;
					SDL_AudioSpec _wanted;

					CPUInst * _cpu;

				public:
					SoundSDL();

					inline void SetCPU(CPUInst * cpu) { this->_cpu = cpu; }
					inline CPUInst * GetCPU() { return this->_cpu; }
					inline double GetInitTimeSound() { return this->_initTimeSound; }

					static SoundSDL* Instance();
					void FillAudio(Uint8 *stream, int len);
			};
		}
	}
}

#endif
