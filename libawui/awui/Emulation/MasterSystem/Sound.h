#pragma once

#include <awui/Object.h>

#include <cstdint>
#include <vector>

struct __OPLL;

namespace awui::Emulation::MasterSystem {
	class Motherboard;

	// PSG SN76489 (variante de Sega) y chip FM YM2413 (Master System japonesa / FM Sound Unit), este con emu2413
	// Referencias: https://www.smspower.org/Development/SN76489 y https://www.smspower.org/Development/YM2413
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
			uint8_t _fmRegisters[0x40]; // Copia de los registros del YM2413 (para restaurarlo al cargar un estado)
			uint8_t _fmAddress;			// Registro seleccionado (puerto 0xF0)
			uint8_t _fmControl;			// Puerto 0xF2. Bits 0-1: 0 = PSG, 1 = FM, 2 = ninguno, 3 = los dos (bit 2 solo se lee)
		} m_saveData;

		static inline bool m_fmEnabled = true; // Ajuste: consola con FM (los juegos lo detectan al arrancar)
		struct __OPLL *m_opll;

		int m_muteSamples; // Muestras que quedan en silencio (tras rebobinar)
		int m_fadeSamples; // Muestras del fundido de entrada ya hechas (al volver el sonido)

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
		virtual ~Sound();

		inline void SetCPU(Motherboard *cpu) { m_cpu = cpu; }
		inline Motherboard *GetCPU() const { return m_cpu; }

		void Reset();
		void WriteByte(Motherboard *cpu, uint8_t value);
		void WriteStereo(Motherboard *cpu, uint8_t value);

		// YM2413 (puertos 0xF0 dirección, 0xF1 dato, 0xF2 control)
		static inline void SetFMEnabled(bool enabled) { m_fmEnabled = enabled; }
		static inline bool IsFMEnabled() { return m_fmEnabled; }
		void WriteFMAddress(uint8_t value);
		void WriteFMData(Motherboard *cpu, uint8_t value);
		void WriteFMControl(Motherboard *cpu, uint8_t value);
		inline uint8_t GetFMControl() const { return m_saveData._fmControl; }
		void EndFrame(Motherboard *cpu);

		// Salto en el tiempo (rebobinado): se descarta el audio pendiente y se silencia un momento, así pulsaciones
		// seguidas no suenan a trozos sueltos, y luego vuelve con un fundido corto (sin chasquido)
		void OnTimeJump();

		static int GetSaveSize();
		void SaveState(uint8_t *data);
		void LoadState(uint8_t *data, int64_t cycle);
	};
} // namespace awui::Emulation::MasterSystem
