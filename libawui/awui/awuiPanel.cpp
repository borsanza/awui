// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiPanel.h"
#include "awuiColor.h"

awuiPanel::awuiPanel() {
  awuiColor * color = awuiColor::FromArgb(0, 0, 255);
  this->SetBackColor(color);
  delete color;
}

awuiPanel::~awuiPanel() {
}

int awuiPanel::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Panel)
		return 1;

	return awuiControl::IsClass(objectClass);
}