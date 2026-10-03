#pragma once

#include <vector>

#include <awui/Emulation/Chip8/KeyMap.h>
#include <awui/String.h>
#include <cstdint>

namespace awui {
	namespace IO {
		class MemoryStream;
	}

	namespace Emulation::Chip8 {
		class Memory {
		  private:
			IO::MemoryStream *m_memory;
			String m_file;
			uint16_t m_startAddress; // Donde empieza el programa: 0x200, o 0x600 en los del ETI-660
			KeyMap m_keyMap;

		  public:
			static constexpr int64_t MaxCapacity = 0x1000000;

			Memory(int32_t capacity);
			virtual ~Memory();

			void LoadRom(const String file);
			inline uint16_t GetStartAddress() const { return m_startAddress; }
			inline const KeyMap &GetKeyMap() const { return m_keyMap; }

			// Los programas del ETI-660 (ordenador australiano de 1981) empiezan en 0x600 en vez de en 0x200. No llevan
			// ninguna marca: se reconocen porque sus saltos, llamadas y LD I apuntan a 0x600 + su tamaño y ninguno a
			// 0x200 + su tamaño (en las 278 ROMs de la colección solo coincide con las 5 del ETI-660)
			static uint16_t DetectStartAddress(const std::vector<uint8_t> &rom);

			uint8_t ReadByte(int64_t pos);
			void WriteByte(int64_t pos, uint8_t value);
			int64_t GetCapacity() const;

			// Para los estados: los primeros size bytes (lo que pase de la memoria reservada, a cero)
			void Save(uint8_t *data, int64_t size) const;
			void Load(const uint8_t *data, int64_t size);

			void Reload();
		};
	} // namespace Emulation::Chip8
} // namespace awui
