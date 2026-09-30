#pragma once

#include <awui/UI/Control.h>

namespace awui::UI::Station {
	class Page : public Control {
	  public:
		Page();

		// Inicio/Fin: primera/última fila. Re Pág/Av Pág: una pantalla de filas (menos una, para no perder la referencia)
		virtual bool OnKeyPress(Input::Keys::Enum key) override;
	};
} // namespace awui::UI::Station
