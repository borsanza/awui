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
				uint8_t _ram[32768];
				uint8_t _keys[8];
				uint8_t _kempston;
			} d;

			Processors::Z80::CPU *_z80;
			ULA *_ula;
			Sound *_sound;
			TapeCorder *_tape; // Para la carga instantánea y el control automático de la cinta (no es suyo)

			// Detección de un cargador (lee el puerto 0xFE en bucle, cientos de veces por frame; el teclado son unas
			// pocas lecturas): la cinta arranca sola si un cargador la espera y se para si nadie la lee
			mutable int64_t _lastEarReadCycle;
			mutable int _loaderReads; // Lecturas seguidas y rápidas del puerto en este frame
			int _framesWithoutLoader;
			bool _tapeWasPlaying;
			void UpdateTapeMotor();

			// No se guarda
			Common::Rom *_rom;

			double _percFrame;

			int8_t _cyclesULA;
			int64_t _cycles;
			int64_t _lastCycles;
			int64_t _lastWriteCycle;
			bool _lastWriteState;
			bool _fast;

			int64_t _lastReadCycle;
			int64_t _countReadCycles;
			bool _lastReadState;

			void Print(const char *str, ...);
			void CheckInterrupts();

			void (*_writeCassetteCB)(int32_t data, void *);
			void *_writeCassetteDataCB;
			int32_t (*_readCassetteCB)(void *);
			void *_readCassetteDataCB;
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
			inline double GetFramePosition() const { return this->_percFrame; }
			static constexpr double FrameSeconds = 1.0 / 59.922743404;
			inline Sound *GetSound() const { return this->_sound; }

			void WriteMemory(uint16_t offset, uint8_t value);
			uint8_t ReadMemory(uint16_t offset) const;
			void WritePort(uint8_t port, uint8_t value);
			uint8_t ReadPort(uint8_t port) const;

			void OnKeyPress(uint8_t row, uint8_t key);
			void OnKeyUp(uint8_t row, uint8_t key);
			void ReleaseAllKeys();
			void OnPadEvent(uint8_t status);

			inline ULA *GetULA() const { return this->_ula; }

			void SetWriteCassetteCB(void (*fun)(int32_t, void *), void *data);
			void SetReadCassetteCB(int32_t (*fun)(void *), void *data);

			void SetFast(bool mode) { this->_fast = mode; };
			bool GetFast() const { return this->_fast; };
			inline void SetTapeCorder(TapeCorder *tape) { this->_tape = tape; }
		};
	} // namespace Spectrum
} // namespace awui::Emulation
