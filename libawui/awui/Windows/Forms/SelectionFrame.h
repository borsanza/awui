#pragma once

namespace awui::Windows::Forms {
	class Control;

	// Marco animado que rodea al control con el foco (uno por formulario). Avanza en el tick y se pinta dentro
	// del padre de ese control (por detrás de él y recortado como él).
	// La posición se guarda relativa a ese padre, así el marco se mueve pegado a una página que se desplaza. Dentro
	// del mismo padre se desliza; al cambiar de padre (otra página, el icono de ajustes...) salta a propósito.
	class SelectionFrame {
	  private:
		const Control *m_parent; // Solo para saber si ha cambiado: puede estar ya borrado, nunca se usa
		float m_left;
		float m_top;
		float m_right;
		float m_bottom;
		bool m_valid;	// Ya tiene posición
		bool m_showing; // Hay un control con foco que lo lleva

	  public:
		SelectionFrame();

		void OnTick(Control *focused, float deltaSeconds);
		void Paint(const Control *parent);
	};
} // namespace awui::Windows::Forms
