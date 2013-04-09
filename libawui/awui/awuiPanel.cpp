// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPanel.h"

#include <awui/Drawing/Color.h>
#include <awui/awuiControl.h>
#include <awui/awuiControlCollection.h>

awuiPanel::awuiPanel() {
}

awuiPanel::~awuiPanel() {
}

int awuiPanel::IsClass(Classes objectClass) const {
	if (objectClass == awui::Panel)
		return 1;

	return awuiControl::IsClass(objectClass);
}

const Size awuiPanel::GetMinimumSize() const {
	Size size = awuiControl::GetMinimumSize();

	Size minSize;
	minSize.SetWidth(0);
	minSize.SetHeight(0);

	int count = this->GetControls()->GetCount();
	for (int i = 0; i < count; i++) {
		awuiControl * control = (awuiControl *)this->GetControls()->Get(i);
		switch (control->GetDock()) {
			case awuiControl::Left:
			case awuiControl::Right:
				minSize.SetWidth(minSize.GetWidth() + control->GetMinimumSize().GetWidth());
				break;
			case awuiControl::Top:
			case awuiControl::Bottom:
				minSize.SetHeight(minSize.GetHeight() + control->GetMinimumSize().GetHeight());
				break;
			case awuiControl::Fill:
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
