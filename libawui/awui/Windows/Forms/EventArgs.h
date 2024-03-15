#pragma once

#include <awui/Object.h>

namespace awui::Windows::Forms {
	class EventArgs : public Object {
	  public:
		EventArgs();
		virtual bool IsClass(Classes objectClass) const override;
	};
} // namespace awui::Windows::Forms
