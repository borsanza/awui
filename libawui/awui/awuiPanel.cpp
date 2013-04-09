// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPanel.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/awuiControlCollection.h>

using namespace awui;
using namespace awui::Drawing;

awuiPanel::awuiPanel() {
}

awuiPanel::~awuiPanel() {
}

int awuiPanel::IsClass(Classes objectClass) const {
	if (objectClass == awui::Panel)
		return 1;

	return Control::IsClass(objectClass);
}

const Size awuiPanel::GetMinimumSize() const {
	Size size = Control::GetMinimumSize();

	Size minSize;
	minSize.SetWidth(0);
	minSize.SetHeight(0);

	int count = this->GetControls()->GetCount();
	for (int i = 0; i < count; i++) {
		Control * control = (Control *)this->GetControls()->Get(i);
		switch (control->GetDock()) {
			case Control::Left:
			case Control::Right:
				minSize.SetWidth(minSize.GetWidth() + control->GetMinimumSize().GetWidth());
				break;
			case Control::Top:
			case Control::Bottom:
				minSize.SetHeight(minSize.GetHeight() + control->GetMinimumSize().GetHeight());
				break;
			case Control::Fill:
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
