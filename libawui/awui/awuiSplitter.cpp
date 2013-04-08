// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitter.h"

#include "awuiColor.h"
#include "awuiMouseEventArgs.h"
#include "awuiSplitContainer.h"

awuiSplitter::awuiSplitter() {
	this->SetBackColor(awuiColor::FromArgb(192, 192, 192));

	this->SetName("awuiSplitter");

	this->SetSize(20, 200);

	this->mouseActive = 0;
}

awuiSplitter::~awuiSplitter() {
}

int awuiSplitter::IsClass(awuiObject::awuiClasses objectClass) const {
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

void awuiSplitter::OnMouseDown(awuiMouseEventArgs * e) {
	if (e->GetButton() == MouseButtons::Left)
		this->mouseActive = 1;
}

void awuiSplitter::OnMouseMove(awuiMouseEventArgs * e) {
	if (!this->mouseActive)
		return;

	if (this->GetParent()->IsClass(awuiObject::SplitContainer)) {
		if (this->orientation == awuiSplitContainer::Vertical)
			((awuiSplitContainer *) this->GetParent())->SetSplitterDistance(this->GetLeft() + e->GetX());
		else
			((awuiSplitContainer *) this->GetParent())->SetSplitterDistance(this->GetTop() + e->GetY());
	}

//	std::cout << "Motion: " << e->GetX() << "x" << e->GetY() << "   " << this->GetName() << std::endl;
}

void awuiSplitter::OnMouseUp(awuiMouseEventArgs * e) {
	this->mouseActive = 0;
}

void awuiSplitter::OnMouseEnter() {
//	std::cout << "OnMouseEnter" << std::endl;
}

void awuiSplitter::OnMouseLeave() {
//	std::cout << "OnMouseLeave" << std::endl;
}
