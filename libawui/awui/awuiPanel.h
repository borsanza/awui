// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Drawing {
		class Size;
	}
}

class awuiPanel : public awui::Windows::Forms::Control {
public:
	awuiPanel();
	~awuiPanel();

	virtual int IsClass(awui::Classes objectClass) const;

	const virtual awui::Drawing::Size GetMinimumSize() const;
};
