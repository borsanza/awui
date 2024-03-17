#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms::Station {
	class Page : public Control {
	  public:
		Page();
		virtual bool IsClass(Classes objectClass) const override;
	};
} // namespace awui::Windows::Forms::Station
