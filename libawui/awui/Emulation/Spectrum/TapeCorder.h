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

			// No se puede copiar: la copia liberaría otra vez los datos del bloque
			TapeBlock(const TapeBlock &) = delete;
			TapeBlock &operator=(const TapeBlock &) = delete;

			void SetByte(int pos, uint8_t value);
			uint8_t GetByte(int pos);
			int GetLength() const;
		};

		class TapeCorder {
		  public:
			// Por dónde va la cinta: va en los estados de la máquina, para que al cargar uno (o rebobinar) la cinta
			// vuelva a donde estaba
			struct Position {
				int32_t block;
				int32_t posByte;
				int32_t posBit;
				int32_t state;
				int32_t cycle;
				uint8_t playing;
			};

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

			// No se puede copiar: la copia liberaría otra vez los bloques de la cinta
			TapeCorder(const TapeCorder &) = delete;
			TapeCorder &operator=(const TapeCorder &) = delete;

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

			Position GetPosition() const;
			void SetPosition(const Position &position);
		};
	} // namespace Emulation::Spectrum
} // namespace awui
