#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms::Station {
	class Page : public Control {
	  public:
		Page();

		// Inicio/Fin: primera/última fila. Re Pág/Av Pág: una pantalla de filas (menos una, para no perder la referencia)
		virtual bool OnKeyPress(Keys::Enum key) override;
	};
} // namespace awui::Windows::Forms::Station
