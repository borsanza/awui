#ifndef _AWUI_EMULATION_MASTERSYSTEM_SOUND_H
#define _AWUI_EMULATION_MASTERSYSTEM_SOUND_H

#include <stdint.h>
#include <SDL.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDFORMAT 2
#define SOUNDSAMPLES 1024
//#define SOUNDSAMPLES 2048

#define TOTALFRAMES 10
#define SOUNDSIZEFRAME ((SOUNDSAMPLES * SOUNDFORMAT) / 2)
#define SOUNDBUFFER (SOUNDSIZEFRAME * TOTALFRAMES)

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class CPUInst;

			class Sound {
				struct Channel {
					float _data;     // 10 bits tono y 3 bits noise
					uint8_t _volume; // 4 bit
					unsigned int _fase;
					int _time;
					unsigned int _buffer[SOUNDBUFFER];
				};

				private:
					uint8_t _data;
					uint8_t _type;
					uint16_t _channel;
					int _frame;
					double _initTimeSound;
					SDL_AudioSpec _wanted;
					Channel _channels[4];

					int GetPosBuffer(CPUInst * cpu);

//					double _lastTime, _actualTime;

				public:
					Sound(CPUInst * cpu);
					~Sound();

					void WriteByte(CPUInst * cpu, uint8_t value);
					void FillAudio(CPUInst * cpu, Uint8 *stream, int len);
			};
		}
	}
}

#endif
