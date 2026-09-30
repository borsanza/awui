#pragma once

#include <stdint.h>
#include <vector>

namespace awui {
	class String;
}

namespace awui {
	namespace Emulation::Spectrum {
		class TapeBlock {
		  private:
			uint8_t *m_data;
			int m_size;

		  public:
			TapeBlock(int size);
			~TapeBlock();

			void SetByte(int pos, uint8_t value);
			uint8_t GetByte(int pos);
			int GetLength() const;
		};

		class TapeCorder {
		  private:
			std::vector<TapeBlock *> *m_list; // Bloques de la cinta (nullptr: no hay cinta cargada)
			int m_posByte;
			int m_posBit;
			int m_block;
			int m_state;
			int32_t m_cycle;
			bool m_playing;

			void (*m_finishCassetteCB)(void *);
			void *m_finishCassetteDataCB;

			void Clear();

		  public:
			TapeCorder();
			~TapeCorder();

			void LoadFile(const String file);

			void Rewind();
			void Play();
			void Stop() { m_playing = false; }
			uint32_t GetNext();
			inline bool IsPlaying() const { return m_playing; }
			// Ya ha sonado entera (o no tiene bloques): no hay nada que reproducir hasta rebobinar
			inline bool IsAtEnd() const { return (m_state >= 8) || !m_list; }

			// Carga instantánea: devuelve el siguiente bloque entero (nullptr si no quedan) y deja la cinta en
			// el siguiente. Al acabar la cinta la para y avisa como si hubiera terminado de sonar
			TapeBlock *TakeNextBlock();

			void SetFinishCassetteCB(void (*fun)(void *), void *data);
		};
	} // namespace Emulation::Spectrum
} // namespace awui
