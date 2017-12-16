// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Splitter.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/SplitContainer.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Splitter::Splitter() {
	this->orientation = SplitContainer::Orientation::Horizontal;
	this->SetBackColor(Color::FromArgb(255, 255, 255));
	this->SetName("Splitter");
	this->SetSize(20, 200);
	this->mouseActive = 0;
}

Splitter::~Splitter() {
}

int Splitter::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Splitter)
		return 1;

	return Control::IsClass(objectClass);
}

SplitContainer::Orientation::Enum Splitter::GetOrientation() {
	return this->orientation;
}

void Splitter::SetOrientation(SplitContainer::Orientation::Enum orientation) {
	this->orientation = orientation;
}

void Splitter::OnMouseDown(MouseEventArgs * e) {
	if (e->GetButton() == MouseButtons::Left)
		this->mouseActive = 1;
}

void Splitter::OnMouseMove(MouseEventArgs * e) {
	if (!this->mouseActive)
		return;

	if (this->GetParent()->IsClass(Classes::SplitContainer)) {
		if (this->orientation == SplitContainer::Orientation::Vertical)
			((SplitContainer *) this->GetParent())->SetSplitterDistance(this->GetLeft() + e->GetX());
		else
			((SplitContainer *) this->GetParent())->SetSplitterDistance(this->GetTop() + e->GetY());
	}

//	std::cout << "Motion: " << e->GetX() << "x" << e->GetY() << "   " << this->GetName() << std::endl;
}

void Splitter::OnMouseUp(MouseEventArgs * e) {
	this->mouseActive = 0;
}

void Splitter::OnMouseEnter() {
//	std::cout << "OnMouseEnter" << std::endl;
}

void Splitter::OnMouseLeave() {
//	std::cout << "OnMouseLeave" << std::endl;
}
