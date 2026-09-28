#pragma once

#include <awui/Emulation/Common/Word.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/Processors/Z80/CPU.h>
#include <awui/String.h>
#include <stdint.h>

namespace awui::Emulation {
	namespace Common {
		class Rom;
	}

	namespace Processors::Z80 {
		class CPU;
	}

	namespace MasterSystem {
		class VDP;
		class Sound;

		enum {
			MAPPER_NONE = 1,
			MAPPER_SEGA = 2,
			MAPPER_CODEMASTERS = 3,
			MAPPER_SG1000 = 4,
		};

		class Motherboard {
		  private:
			struct saveData {
				double _frameAccumulator; // Fracción de frame de la consola pendiente (PAL: 49.70 frames por cada 59.92 ticks)
				uint8_t _controlbyte;
				uint8_t _frame0;
				uint8_t _frame1;
				uint8_t _frame2;
				uint8_t _mapper;
				uint8_t _pad1;
				uint8_t _pad2;
				bool _wantPause : 1;
				bool _codemastersRam : 1; // Ernie Els Golf: 8KB de RAM en 0xA000-0xBFFF
				Word _addressBus;
				Ports _ports;
				uint8_t _boardram[32768];
				uint8_t _ram[8192];
			} m_saveData;

			// No se guarda
			bool m_showLog : 1;
			bool m_showLogInt : 1;
			bool m_showNotImplemented : 1;
			Common::Rom *m_rom;
			bool m_startButton; // Game Gear: botón START (puerto 0x00)

			VDP *m_vdp;
			Sound *m_sound;
			Processors::Z80::CPU m_z80;
			double m_seconds; // Tiempo real pendiente de emular (siempre menor que un tick salvo tras un parón)

			void CheckInterrupts();
			uint16_t GetBoardRamOffset(uint16_t pos) const;
			bool IsCodemastersRom() const;
			void DoTick();

		  public:
			Motherboard();
			virtual ~Motherboard();

			void LoadRom(const String file);
			void OnTick(float deltaSeconds);
			bool IsEndlessLoop() const;

			uint16_t GetAddressBus() const;
			void SetAddressBus(uint16_t);

			void Reset();

			void CallPaused();

			// Botón de pausa: en Master System genera la NMI al pulsarlo; en Game Gear es el botón START
			void SetPauseButton(bool pressed);
			inline bool GetStartButton() const { return m_startButton; }
			bool IsGameGear() const;

			inline VDP *GetVDP() const { return m_vdp; }
			inline Sound *GetSound() const { return m_sound; }
			inline void SetPad1(uint8_t pad1) { m_saveData._pad1 = pad1; }
			inline void SetPad2(uint8_t pad2) { m_saveData._pad2 = pad2; }
			inline uint8_t GetPad1() const { return m_saveData._pad1; }
			inline uint8_t GetPad2() const { return m_saveData._pad2; }

			uint32_t GetCRC32();
			void SetMapper(uint8_t mapper);
			inline uint16_t GetPC() const { return m_z80.GetPC(); }

			static int GetSaveSize();
			void LoadState(uint8_t *data);
			void SaveState(uint8_t *data);

			inline int64_t GetCycles() const { return m_z80.GetCycles(); }
			void RunOpcode();

			void WriteMemory(uint16_t pos, uint8_t value);
			uint8_t ReadMemory(uint16_t pos) const;
			void WritePort(uint8_t port, uint8_t value);
			uint8_t ReadPort(uint8_t port);
		};
	} // namespace MasterSystem
} // namespace awui::Emulation
