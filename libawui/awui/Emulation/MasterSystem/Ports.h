#pragma once

#include <stdint.h>

namespace awui::Emulation::MasterSystem {
	class Motherboard;

	class Ports {
	  private:
		uint8_t _region;
		uint8_t _maskRegion;
		bool _getRegion : 1;

	  public:
		Ports();

		void WriteByte(Motherboard *cpu, uint8_t port, uint8_t value);
		uint8_t ReadByte(Motherboard *cpu, uint8_t port) const;
	};
} // namespace awui::Emulation::MasterSystem
