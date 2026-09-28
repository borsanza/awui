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

		// Estado del chip: se guarda en los estados de la placa (rebobinado)
		struct saveData {
			uint16_t _registers[4]; // 0-2: periodo del tono (10 bits), 3: control del ruido (3 bits)
			uint8_t _volumes[4];	// Atenuación (4 bits, 0xF = silencio)
			int _counters[4];
			int8_t _outputs[4]; // +1 / -1
			bool _noiseToggle;
			uint16_t _lfsr;
			uint8_t _latchedChannel;
			bool _latchedVolume;
			int _pendingCycles; // Ciclos de CPU que aún no llegan a un paso del chip (16)
			uint8_t _stereo; // Game Gear (puerto 0x06): bits 7-4 canales 3-0 a la izquierda, bits 3-0 a la derecha
		} m_saveData;

		int64_t m_lastCycle;
		double m_ticksPerSample;
		double m_tickPos;
		int m_sampleSumLeft;
		int m_sampleSumRight;
		int m_sampleTicks;
		std::vector<int16_t> m_samples; // Estéreo intercalado: izquierda, derecha

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

		static int GetSaveSize();
		void SaveState(uint8_t *data);
		void LoadState(uint8_t *data, int64_t cycle);
	};
} // namespace awui::Emulation::MasterSystem
