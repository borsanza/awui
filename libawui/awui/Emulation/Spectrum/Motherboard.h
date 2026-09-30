#pragma once

#include <awui/String.h>
#include <cstdint>

namespace awui::Emulation {
	namespace Common {
		class Rom;
	}

	namespace Processors::Z80 {
		class CPU;
	}

	namespace Spectrum {
		class Sound;
		class TapeCorder;
		class ULA;

		class Motherboard {
		  private:
			struct saveData {
				uint8_t ram[32768];
				uint8_t keys[8];
				uint8_t kempston;
			} m_saveData;

			Processors::Z80::CPU *m_z80;
			ULA *m_ula;
			Sound *m_sound;
			TapeCorder *m_tape; // Para la carga instantánea y el control automático de la cinta (no es suyo)

			// Detección de un cargador (lee el puerto 0xFE en bucle, cientos de veces por frame; el teclado son unas
			// pocas lecturas): la cinta arranca sola si un cargador la espera y se para si nadie la lee
			mutable int64_t m_lastEarReadCycle;
			mutable int m_loaderReads; // Lecturas seguidas y rápidas del puerto en este frame
			int m_framesWithoutLoader;
			bool m_tapeWasPlaying;
			void UpdateTapeMotor();

			// No se guarda
			Common::Rom *m_rom;

			double m_percFrame;

			int8_t m_cyclesULA;
			int64_t m_cycles;
			int64_t m_lastCycles;
			int64_t m_lastWriteCycle;
			bool m_lastWriteState;
			bool m_fast;

			int64_t m_lastReadCycle;
			int64_t m_countReadCycles;
			bool m_lastReadState;

			void Print(const char *str, ...);
			void CheckInterrupts();

			void (*m_writeCassetteCB)(int32_t data, void *);
			void *m_writeCassetteDataCB;
			int32_t (*m_readCassetteCB)(void *);
			void *m_readCassetteDataCB;
			void ProcessCassette();
			bool FlashLoad();

		  public:
			Motherboard();
			virtual ~Motherboard();

			void LoadRom(const String file);
			void OnTick();

			uint16_t GetAddressBus() const;
			void SetAddressBus(uint16_t);

			void Reset();

			void PrintLog();

			uint32_t GetCRC32();
			void SetMapper(uint8_t mapper);

			static int GetSaveSize();
			void LoadState(uint8_t *data);
			void SaveState(uint8_t *data);

			// Parte del frame actual ya emulada (0..1): sitúa los cambios del altavoz dentro del frame
			inline double GetFramePosition() const { return m_percFrame; }
			static constexpr double FrameSeconds = 1.0 / 59.922743404;
			inline Sound *GetSound() const { return m_sound; }

			void WriteMemory(uint16_t offset, uint8_t value);
			uint8_t ReadMemory(uint16_t offset) const;
			void WritePort(uint8_t port, uint8_t value);
			uint8_t ReadPort(uint8_t port) const;

			void OnKeyPress(uint8_t row, uint8_t key);
			void OnKeyUp(uint8_t row, uint8_t key);
			void ReleaseAllKeys();
			void OnPadEvent(uint8_t status);

			inline ULA *GetULA() const { return m_ula; }

			void SetWriteCassetteCB(void (*fun)(int32_t, void *), void *data);
			void SetReadCassetteCB(int32_t (*fun)(void *), void *data);

			void SetFast(bool mode) { m_fast = mode; };
			bool GetFast() const { return m_fast; };
			inline void SetTapeCorder(TapeCorder *tape) { m_tape = tape; }
		};
	} // namespace Spectrum
} // namespace awui::Emulation
