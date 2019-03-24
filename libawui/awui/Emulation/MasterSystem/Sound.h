#pragma once

#include <stdint.h>
#include <blargg/Sms_Apu.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDSAMPLES 1024
//#define SOUNDSAMPLES 2048

#define TOTALFRAMES 3
#define SOUNDBUFFER (SOUNDSAMPLES * TOTALFRAMES)

#include <awui/Object.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Motherboard;

			class Sound : public Object {
				private:
					Motherboard * _cpu;
					Blip_Buffer buf;
					Sms_Apu apu;

				public:
					Sound();

					inline void SetCPU(Motherboard * cpu) { this->_cpu = cpu; }
					inline Motherboard * GetCPU() { return this->_cpu; }

					void WriteData(Motherboard * cpu, uint8_t value);
					void WriteGGStereo(Motherboard * cpu, uint8_t value);
			};
		}
	}
}
