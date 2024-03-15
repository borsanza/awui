#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms::Station::Browser {
	class Page : public Control {
	  public:
		Page();
		virtual bool IsClass(Classes objectClass) const override;
	};
} // namespace awui::Windows::Forms::Station::Browser
