#pragma once

#include <awui/Emulation/Chip8/Opcode.h>
#include <awui/String.h>

namespace awui {
	class Random;

	namespace Emulation::Chip8 {
		class Input;
		class Memory;
		class Registers;
		class Screen;
		class Stack;
		class Sound;

		class CPU {
		  private:
			int16_t m_pc;
			Screen *m_screen;
			Registers *m_registers;
			Memory *m_memory;
			Random *m_random;
			Input *m_input;
			Stack *m_stack;
			Sound *m_sound;
			uint8_t m_chip8mode;
			uint8_t m_delayTimer;
			uint8_t m_soundTimer;
			uint16_t m_spriteWidth;
			uint16_t m_spriteHeight;
			uint16_t m_frameCounter;

			float m_seconds;
			float m_nextTick;

			int m_finished;
			bool m_imageUpdated;

			bool m_firstTime;
			Opcode m_opcode;

			uint32_t *m_colors;

			int RunOpcode(int iteration);
			void ChangeResolution(uint16_t width, uint16_t height);
			void DoTick();

		  public:
			CPU();
			virtual ~CPU();

			void LoadRom(const String file);
			void OnTick(float deltaSeconds);

			Screen *GetScreen();

			bool GetImageUpdated() const;
			void SetImageUpdated(bool mode);

			uint8_t GetChip8Mode() const;

			void Reset();

			void KeyDown(uint8_t key);
			void KeyUp(uint8_t key);
		};
	} // namespace Emulation::Chip8
} // namespace awui
