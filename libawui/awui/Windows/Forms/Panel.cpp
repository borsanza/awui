// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Panel.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Control.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Panel::Panel() {
}

Panel::~Panel() {
}

bool Panel::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Panel) {
		return true;
	}

	return Control::IsClass(objectClass);
}

const Size Panel::GetMinimumSize() const {
	Size size = Control::GetMinimumSize();

	Size minSize;
	minSize.SetWidth(0);
	minSize.SetHeight(0);

	int count = this->GetControls()->GetCount();
	for (int i = 0; i < count; i++) {
		Control * control = (Control *)this->GetControls()->Get(i);
		switch (control->GetDock()) {
			case DockStyle::None:
				break;
			case DockStyle::Left:
			case DockStyle::Right:
				minSize.SetWidth(minSize.GetWidth() + control->GetMinimumSize().GetWidth());
				break;
			case DockStyle::Top:
			case DockStyle::Bottom:
				minSize.SetHeight(minSize.GetHeight() + control->GetMinimumSize().GetHeight());
				break;
			case DockStyle::Fill:
				minSize.SetWidth(minSize.GetWidth() + control->GetMinimumSize().GetWidth());
				minSize.SetHeight(minSize.GetHeight() + control->GetMinimumSize().GetHeight());
				break;
		}
	}

	if (minSize.GetWidth() < size.GetWidth())
		minSize.SetWidth(size.GetWidth());

	if (minSize.GetHeight() < size.GetHeight())
		minSize.SetHeight(size.GetHeight());

	return minSize;
}
