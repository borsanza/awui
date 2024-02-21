#pragma once

#include <awui/Emulation/Chip8/Opcode.h>
#include <awui/String.h>

namespace awui {
	class Random;

	namespace Emulation {
		namespace Chip8 {
			class Input;
			class Memory;
			class Registers;
			class Screen;
			class Stack;
			class Sound;

			class CPU {
				private:
					int16_t _pc;
					Screen * _screen;
					Registers * _registers;
					Memory * _memory;
					Random * _random;
					Input * _input;
					Stack * _stack;
					Sound * _sound;
					uint8_t _chip8mode;
					uint8_t _delayTimer;
					uint8_t _soundTimer;
					uint16_t _spriteWidth;
					uint16_t _spriteHeight;
					uint16_t _frameCounter;

					float m_seconds;
					float m_nextTick;

					int _finished;
					bool _imageUpdated;

					bool _firstTime;
					Opcode _opcode;

					uint32_t * _colors;

					int RunOpcode(int iteration);
					void ChangeResolution(uint16_t width, uint16_t height);
					void DoTick();

				public:
					CPU();
					virtual ~CPU();

					void LoadRom(const String file);
					void OnTick(float deltaSeconds);

					Screen * GetScreen();

					bool GetImageUpdated() const;
					void SetImageUpdated(bool mode);

					uint8_t GetChip8Mode() const;

					void Reset();

					void KeyDown(uint8_t key);
					void KeyUp(uint8_t key);
			};
		}
	}
}
