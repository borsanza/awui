// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPanel.h"
#include "awuiColor.h"
#include "awuiControl.h"
#include "awuiControlCollection.h"

awuiPanel::awuiPanel() {
}

awuiPanel::~awuiPanel() {
}

int awuiPanel::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Panel)
		return 1;

	return awuiControl::IsClass(objectClass);
}

const awuiSize awuiPanel::GetMinimumSize() const {
	awuiSize size = awuiControl::GetMinimumSize();

	awuiSize minSize;
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
