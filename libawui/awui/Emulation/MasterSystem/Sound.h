#ifndef _AWUI_EMULATION_MASTERSYSTEM_SOUND_H
#define _AWUI_EMULATION_MASTERSYSTEM_SOUND_H

#include <stdint.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDFORMAT 1
#define SOUNDSAMPLES 1024
//#define SOUNDSAMPLES 2048

#define TOTALFRAMES 3
#define SOUNDSIZEFRAME ((SOUNDSAMPLES * SOUNDFORMAT) / 2)
#define SOUNDBUFFER (SOUNDSIZEFRAME * TOTALFRAMES)

#include <awui/Object.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			struct Sample {
				int8_t _volume;
				uint16_t _tone;
				bool _changeTone;
				bool _changeVolume;
			};

			struct Channel {
				bool _useModulation;
				uint16_t _register;

				uint16_t _tone;     // 10 bits tono y 3 bits noise
				uint8_t _volume; // 4 bit
				unsigned int _fase;

				Sample _buffer[SOUNDBUFFER];
				Sample _last;
				int _count;
			};

			class CPUInst;

			class Sound : public Object {
				private:
					uint16_t _channel;
					uint8_t _type; // 1: Volumen, 0: Tone/Noise

					int GetPosBuffer(CPUInst * cpu);
					CPUInst * _cpu;

				public:
					uint16_t _noiseData;
					Channel _channels[4];

					Sound();

					inline void SetCPU(CPUInst * cpu) { this->_cpu = cpu; }
					inline CPUInst * GetCPU() { return this->_cpu; }

					void WriteByte(CPUInst * cpu, uint8_t value);
			};
		}
	}
}

#endif
