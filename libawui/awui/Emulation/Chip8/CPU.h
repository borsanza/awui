#pragma once

#include <cstdint>
#include <vector>

#include <awui/Emulation/Chip8/Opcode.h>
#include <awui/String.h>

namespace awui {
	class Random;

	namespace Emulation::Chip8 {
		class Input;
		struct KeyMap;
		class Memory;
		class Registers;
		class Screen;
		class Stack;
		class Sound;

		class CPU {
		  private:
			int16_t m_pc;
			Screen *m_screen;	   // Donde se dibuja
			Screen *m_frontScreen; // MegaChip: lo que se ve (CLS copia aquí lo dibujado y empieza otro frame)
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

			// ROM terminada (salto a sí misma o 00FD) y segundos que lleva así
			bool m_finished;
			float m_finishedSeconds;
			uint32_t m_timesFinished; // Veces que ha terminado desde que se cargó (para avisar en pantalla)
			static inline bool s_restartWhenFinished = true; // Ajuste: volver a empezar al terminar
			bool m_imageUpdated;

			bool m_firstTime;
			Opcode m_opcode;

			// MegaChip: paleta de 256 colores (el 0 es transparente) y, por cada píxel de la pantalla, el índice de la
			// paleta con el que se pintó (para las colisiones, que van por color y no por píxel encendido)
			uint32_t m_colors[256];
			std::vector<uint8_t> m_colorIndices;
			// Color con el que choca un sprite (09nn); -1: cualquier color distinto de 0, como dice la especificación
			int m_collisionColor;

			void ClearColorIndices();

			int RunOpcode(int iteration);
			void ChangeResolution(uint16_t width, uint16_t height);
			void DoTick();

		  public:
			CPU();
			virtual ~CPU();

			void LoadRom(const String file);
			void OnTick(float deltaSeconds);

			// Segundos que se queda en la pantalla final antes de volver a empezar
			static constexpr float RestartSeconds = 5.0f;
			static inline void SetRestartWhenFinished(bool enabled) { s_restartWhenFinished = enabled; }
			static inline bool GetRestartWhenFinished() { return s_restartWhenFinished; }
			inline uint32_t GetTimesFinished() const { return m_timesFinished; }

			Screen *GetScreen();
			inline Sound *GetSound() const { return m_sound; }
			const KeyMap &GetKeyMap() const;

			bool GetImageUpdated() const;
			void SetImageUpdated(bool mode);

			uint8_t GetChip8Mode() const;

			void Reset();

			void KeyDown(uint8_t key);
			void KeyUp(uint8_t key);
		};
	} // namespace Emulation::Chip8
} // namespace awui
