// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitter.h"

#include "awuiColor.h"
#include "awuiMouseEventArgs.h"
#include "awuiSplitContainer.h"

awuiSplitter::awuiSplitter() {
  awuiColor * color = awuiColor::FromArgb(0, 255, 0);
  this->SetBackColor(color);
  delete color;
	
	this->SetName("awuiSplitter");

	this->SetSize(20, 200);
}

awuiSplitter::~awuiSplitter() {
}

int awuiSplitter::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Splitter)
		return 1;

	return awuiControl::IsClass(objectClass);
}

awuiSplitContainer::Orientation awuiSplitter::GetOrientation() {
	return this->orientation;
}

void awuiSplitter::SetOrientation(awuiSplitContainer::Orientation orientation) {
	this->orientation = orientation;
}

#include <iostream>

void awuiSplitter::OnMouseMove(awuiMouseEventArgs * e) {
	if (this->GetParent()->IsClass(awuiObject::SplitContainer)) {
		((awuiSplitContainer *) this->GetParent())->SetSplitterDistance(
			((awuiSplitContainer *) this->GetParent())->GetSplitterDistance() + e->GetX()
		);
	}

	std::cout << "Motion: " << e->GetX() << "x" << e->GetY() << "    " << this->GetName() << std::endl;
}