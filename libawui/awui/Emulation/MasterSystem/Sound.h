#ifndef _AWUI_EMULATION_MASTERSYSTEM_SOUND_H
#define _AWUI_EMULATION_MASTERSYSTEM_SOUND_H

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

			class Sound {

				struct Sample {
					int8_t _volume;
					uint16_t _tone;
					bool _changeTone;
					bool _changeVolume;
				};

				struct Channel {
					uint16_t _register;

					uint16_t _tone;     // 10 bits tono y 3 bits noise
					uint8_t _volume; // 4 bit
					unsigned int _fase;

					Sample _buffer[SOUNDBUFFER];
					Sample _last;
					int _count;
				};

				private:
					static Sound* _instance;
					uint16_t _channel;
					uint8_t _type; // 1: Volumen, 0: Tone/Noise
					int _frame;
					double _initTimeSound;
					SDL_AudioSpec _wanted;
					Channel _channels[4];

					int GetPosBuffer(CPUInst * cpu);

					CPUInst * _cpu;

				public:
					Sound();
					~Sound();

					inline void SetCPU(CPUInst * cpu) { this->_cpu = cpu; }
					inline CPUInst * GetCPU() { return this->_cpu; }

					static Sound* Instance();
					void WriteByte(CPUInst * cpu, uint8_t value);
					void FillAudio(Uint8 *stream, int len);
			};
		}
	}
}

#endif
