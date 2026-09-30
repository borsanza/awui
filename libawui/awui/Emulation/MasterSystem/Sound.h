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
	// se envían las muestras generadas a Common::AudioOutput.
	class Sound : public Object {
	  private:
		Motherboard *m_cpu;

		// Estado del chip: se guarda en los estados de la placa (rebobinado)
		struct saveData {
			uint16_t registers[4]; // 0-2: periodo del tono (10 bits), 3: control del ruido (3 bits)
			uint8_t volumes[4];	// Atenuación (4 bits, 0xF = silencio)
			int counters[4];
			int8_t outputs[4]; // +1 / -1
			bool noiseToggle;
			uint16_t lfsr;
			uint8_t latchedChannel;
			bool latchedVolume;
			int pendingCycles; // Ciclos de CPU que aún no llegan a un paso del chip (16)
			uint8_t stereo; // Game Gear (puerto 0x06): bits 7-4 canales 3-0 a la izquierda, bits 3-0 a la derecha
			uint8_t fmAddress;			// Registro seleccionado (puerto 0xF0)
			uint8_t fmControl;			// Puerto 0xF2. Bits 0-1: 0 = PSG, 1 = FM, 2 = ninguno, 3 = los dos (bit 2 solo se lee)
		} m_saveData;

		static inline uint8_t s_disabledChannels = 0x00; // Canales del PSG silenciados (bit n: canal n)
		static inline bool s_fmEnabled = true; // Ajuste: consola con FM (los juegos lo detectan al arrancar)
		struct __OPLL *m_opll;

		bool m_reverse;	   // Rebobinando: el audio de cada frame se envía al revés
		int m_fadeSamples; // Muestras del fundido de entrada ya hechas (al cambiar de sentido)

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
		// Canales del PSG que suenan (teclas 1-4 y ajustes): se pueden silenciar uno a uno
		static void ToggleChannel(int channel) { s_disabledChannels ^= 1 << channel; }
		static void SetChannelEnabled(int channel, bool enabled) {
			if (enabled)
				s_disabledChannels &= ~(1 << channel);
			else
				s_disabledChannels |= 1 << channel;
		}
		static bool IsChannelEnabled(int channel) { return (s_disabledChannels & (1 << channel)) == 0; }

		Sound();
		virtual ~Sound();

		inline void SetCPU(Motherboard *cpu) { m_cpu = cpu; }
		inline Motherboard *GetCPU() const { return m_cpu; }

		void Reset();
		void WriteByte(Motherboard *cpu, uint8_t value);
		void WriteStereo(Motherboard *cpu, uint8_t value);

		// YM2413 (puertos 0xF0 dirección, 0xF1 dato, 0xF2 control)
		static inline void SetFMEnabled(bool enabled) { s_fmEnabled = enabled; }
		static inline bool IsFMEnabled() { return s_fmEnabled; }
		void WriteFMAddress(uint8_t value);
		void WriteFMData(Motherboard *cpu, uint8_t value);
		void WriteFMControl(Motherboard *cpu, uint8_t value);
		inline uint8_t GetFMControl() const { return m_saveData.fmControl; }
		void EndFrame(Motherboard *cpu);

		// Rebobinado: cada frame emulado se oye al revés; como se rebobina frame a frame hacia atrás, el resultado
		// es el sonido invertido y continuo. Al cambiar de sentido hay un fundido corto para que no chasquee
		void SetReverse(bool reverse);

		// El estado incluye el del YM2413 entero (su estructura de emu2413). Sus punteros se guardan como índices
		// y se rehacen al cargar, así que sirve también para estados en fichero

		static int GetSaveSize();
		void SaveState(uint8_t *data);
		void LoadState(uint8_t *data, int64_t cycle);
	};
} // namespace awui::Emulation::MasterSystem
