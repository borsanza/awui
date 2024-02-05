#pragma once

#include <stdint.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDSAMPLES 512
//#define SOUNDSAMPLES 1024
//#define SOUNDSAMPLES 2048

#define TOTALFRAMES 3
#define SOUNDBUFFER (SOUNDSAMPLES * TOTALFRAMES)

#include <awui/Object.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			struct Sample {
				int8_t _volume;
				uint16_t _tone;
				uint8_t _changeTone:1;
				uint8_t _changeVolume:1;
			};

			struct Channel {
				bool _useModulation;
				uint16_t _register;

				uint16_t _tone;  // 10 bits tono y 3 bits noise
				uint8_t _volume; // 4 bit
				unsigned int _fase;

				Sample _buffer[SOUNDBUFFER];
				Sample _last;
				unsigned int _count;
			};

			class Motherboard;

			class Sound : public Object {
				private:
					uint16_t _channel;
					uint8_t _type; // 1: Volumen, 0: Tone/Noise

					int GetPosBuffer(Motherboard * cpu);
					Motherboard * _cpu;

				public:
					uint16_t _noiseData;
					Channel _channels[4];

					Sound();

					inline void SetCPU(Motherboard * cpu) { this->_cpu = cpu; }
					inline Motherboard * GetCPU() { return this->_cpu; }

					void WriteByte(Motherboard * cpu, uint8_t value);
			};
		}
	}
}
