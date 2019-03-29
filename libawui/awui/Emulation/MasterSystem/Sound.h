#pragma once

#include <stdint.h>
#include <blargg/Sms_Apu.h>

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
