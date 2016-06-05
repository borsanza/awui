#pragma once

#include <stdint.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDFORMAT 1
#define SOUNDSAMPLES 1024

#define TOTALFRAMES 8
#define SOUNDSIZEFRAME ((SOUNDSAMPLES * SOUNDFORMAT) / 2)
#define SOUNDBUFFER (SOUNDSIZEFRAME * TOTALFRAMES)

#include <awui/Object.h>

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			class Motherboard;

			class Sound : public Object {
				private:
					Motherboard * _cpu;

					int GetPosBuffer(Motherboard * cpu);

				public:
					int16_t _buffer[SOUNDBUFFER];

					Sound();

					inline void SetCPU(Motherboard * cpu) { this->_cpu = cpu; }
					inline Motherboard * GetCPU() { return this->_cpu; }

					void WriteByte(Motherboard * cpu, int16_t value);

					void WriteSound(Motherboard * cpu, int sound);
			};
		}
	}
}
