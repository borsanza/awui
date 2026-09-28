#pragma once

#include <awui/Object.h>

#include <cstdint>
#include <vector>

namespace awui::Emulation::MasterSystem {
	class Motherboard;

	// PSG SN76489 (variante de Sega)
	// Referencia: https://www.smspower.org/Development/SN76489
	//
	// Las escrituras de registros se aplican en el ciclo de CPU exacto en el que ocurren:
	// antes de cada escritura se genera el audio hasta ese ciclo, y al final de cada frame
	// se envían las muestras generadas a SoundSDL.
	class Sound : public Object {
	  private:
		Motherboard *m_cpu;

		uint16_t m_registers[4]; // 0-2: periodo del tono (10 bits), 3: control del ruido (3 bits)
		uint8_t m_volumes[4];	 // Atenuación (4 bits, 0xF = silencio)
		int m_counters[4];
		int8_t m_outputs[4]; // +1 / -1
		bool m_noiseToggle;
		uint16_t m_lfsr;

		uint8_t m_latchedChannel;
		bool m_latchedVolume;

		int64_t m_lastCycle;
		int m_pendingCycles;
		double m_ticksPerSample;
		double m_tickPos;
		int m_sampleSumLeft;
		int m_sampleSumRight;
		int m_sampleTicks;
		std::vector<int16_t> m_samples; // Estéreo intercalado: izquierda, derecha
		uint8_t m_stereo;				 // Game Gear (puerto 0x06): bits 7-4 canales 3-0 a la izquierda, bits 3-0 a la derecha

		void Tick();
		void Render(int64_t cycle);

	  public:
		Sound();

		inline void SetCPU(Motherboard *cpu) { m_cpu = cpu; }
		inline Motherboard *GetCPU() const { return m_cpu; }

		void Reset();
		void WriteByte(Motherboard *cpu, uint8_t value);
		void WriteStereo(Motherboard *cpu, uint8_t value);
		void EndFrame(Motherboard *cpu);
	};
} // namespace awui::Emulation::MasterSystem
