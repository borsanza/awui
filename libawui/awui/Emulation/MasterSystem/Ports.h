#pragma once

#include <stdint.h>

namespace awui::Emulation::MasterSystem {
	class Motherboard;

	class Ports {
	  private:
		uint8_t m_ioControl; // Último valor escrito en el puerto 0x3F (control de E/S)

		uint8_t GetPinLevel(uint8_t directionBit, uint8_t outputBit, bool isTH) const;
		static bool HasFM(Motherboard *cpu);

	  public:
		Ports();

		void WriteByte(Motherboard *cpu, uint8_t port, uint8_t value);
		uint8_t ReadByte(Motherboard *cpu, uint8_t port) const;
	};
} // namespace awui::Emulation::MasterSystem
