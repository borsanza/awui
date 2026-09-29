#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

namespace awui::Emulation::Common {
	// Historial de estados para rebobinar, pensado para guardar uno por frame.
	// Solo se guarda entero el estado actual; de los anteriores, lo que cambia entre dos seguidos (XOR, que casi
	// todo son ceros, comprimido por tramos de ceros). Retroceder deshace el último cambio y avanzar lo rehace.
	// Al llegar al límite de memoria se olvidan los más antiguos.
	class RewindBuffer {
	  private:
		size_t m_stateSize;
		size_t m_maxBytes;
		size_t m_bytes; // Memoria que ocupan los cambios guardados (atrás y adelante)

		std::vector<uint8_t> m_current; // Estado en el que está el historial
		bool m_hasCurrent;
		std::deque<std::vector<uint8_t>> m_back;	// Cambios hacia atrás (el último es el más reciente)
		std::vector<std::vector<uint8_t>> m_forward; // Cambios deshechos, para volver a avanzar

		void Compress(const uint8_t *state, std::vector<uint8_t> &out) const;
		void ApplyDelta(const std::vector<uint8_t> &delta);

	  public:
		RewindBuffer(size_t stateSize, size_t maxBytes);

		void Clear();

		// Nuevo estado tras el actual. Descarta lo que se pudiera rehacer: empieza otra línea de tiempo
		void Push(const uint8_t *state);

		// Pasan al estado anterior / siguiente y lo copian en state. false si no hay más
		bool Back(uint8_t *state);
		bool Forward(uint8_t *state);

		inline size_t GetBackCount() const { return m_back.size(); }
		inline size_t GetBytes() const { return m_bytes; }
	};
} // namespace awui::Emulation::Common
