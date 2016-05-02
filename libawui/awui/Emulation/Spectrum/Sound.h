#ifndef _AWUI_EMULATION_SPECTRUM_SOUND_H
#define _AWUI_EMULATION_SPECTRUM_SOUND_H

#include <stdint.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDFORMAT 1
#define SOUNDSAMPLES 1024

#define TOTALFRAMES 4
#define SOUNDSIZEFRAME ((SOUNDSAMPLES * SOUNDFORMAT) / 2)
#define SOUNDBUFFER (SOUNDSIZEFRAME * TOTALFRAMES)

#include <awui/Object.h>

namespace awui {
	namespace Emulation {
		namespace Spectrum {
			struct Sample {
				int16_t _signal;
			};

			struct Channel {
				Sample _buffer[SOUNDBUFFER];
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
					Channel _channels;

					Sound();

					inline void SetCPU(Motherboard * cpu) { this->_cpu = cpu; }
					inline Motherboard * GetCPU() { return this->_cpu; }

					void WriteByte(Motherboard * cpu, int16_t value);

					void WriteSound(Motherboard * cpu, int sound);
			};
		}
	}
}

#endif
