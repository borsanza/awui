#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms {
	class Keyboard : public Control {
	  public:
		Keyboard();
		virtual ~Keyboard() = default;

		virtual bool IsClass(Classes objectClass) const override;
	};
} // namespace awui::Windows::Forms
