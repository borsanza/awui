// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "SplitContainer.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/Splitter.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

SplitContainer::SplitContainer() {
	this->splitterDistance = 100;
	this->splitterIncrement = 1;
	this->splitterWidth = 4;
	this->orientation = Orientation::Vertical;

	this->SetName("SplitContainer");

	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));

	this->panel1 = new Panel();
	this->splitter = new Splitter();
	this->splitter->SetOrientation(this->orientation);
	this->panel2 = new Panel();
	this->GetControls()->Add(this->panel1);
	this->GetControls()->Add(this->splitter);
	this->GetControls()->Add(this->panel2);

	this->SetSize(200, 200);
}

SplitContainer::~SplitContainer() {
}

int SplitContainer::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::SplitContainer)
		return 1;

	return Control::IsClass(objectClass);
}

Panel * SplitContainer::GetPanel1() {
	return this->panel1;
}

Panel * SplitContainer::GetPanel2() {
	return this->panel2;
}

void SplitContainer::SetLocationW(Control * control, int pos) {
	if (this->orientation == Orientation::Vertical)
		control->SetLocation(pos, 0);
	else
		control->SetLocation(0, pos);
}

void SplitContainer::SetSizeW(Control * control, int size, int substract) {
	if (this->orientation == Orientation::Vertical) {
		if (substract)
			size = this->GetWidth() - size;

		control->SetSize(size, this->GetHeight());
	} else {
		if (substract)
			size = this->GetHeight() - size;

		control->SetSize(this->GetWidth(), size);
	}
}

int SplitContainer::GetSizeW(Control * control) const {
	if (this->orientation == Orientation::Vertical)
		return control->GetWidth();
	else
		return control->GetHeight();
}

int SplitContainer::GetMinimumSizeW(Control * control) const {
	if (this->orientation == Orientation::Vertical)
		return control->GetMinimumSize().GetWidth();
	else
		return control->GetMinimumSize().GetHeight();
}

void SplitContainer::RecalculatePositions() {
	int distance = this->splitterDistance;
	int x2 = this->splitterWidth >> 1;
	int x1 = this->splitterWidth - x2;

	int size = (this->orientation == Orientation::Vertical)? this->GetWidth() : this->GetHeight();

	int minSize = this->GetMinimumSizeW(this->panel1);
	if (distance < (minSize + x1))
		distance = (minSize + x1);

	minSize = this->GetMinimumSizeW(this->panel2);
	if (distance > (size - minSize - x1))
		distance = (size - minSize - x1);

	this->SetLocationW(this->panel1, 0);
	this->SetSizeW(this->panel1, distance - x1, 0);

	this->SetLocationW(this->splitter, distance - x1);
	this->SetSizeW(this->splitter, this->splitterWidth, 0);

	this->SetLocationW(this->panel2, distance + x2);
	this->SetSizeW(this->panel2, (distance + x2), 1);
}

float SplitContainer::GetSplitterDistance() const {
	return this->splitterDistance;
}

void SplitContainer::SetSplitterDistance(int distance) {
	this->splitterDistance = distance;
	this->RecalculatePositions();
}

int SplitContainer::GetSplitterIncrement() const {
	return this->splitterIncrement;
}

void SplitContainer::SetSplitterIncrement(int increment) {
	this->splitterIncrement = increment;
}

int SplitContainer::GetSplitterWidth() const {
	return this->splitterWidth;
}

void SplitContainer::SetSplitterWidth(int width) {
	this->splitterWidth = width;
}

SplitContainer::Orientation::Enum SplitContainer::GetOrientation() const {
	return this->orientation;
}

void SplitContainer::SetOrientation(Orientation::Enum orientation) {
	this->orientation = orientation;
	this->splitter->SetOrientation(orientation);
	this->RecalculatePositions();
}

void SplitContainer::Layout() {
	this->RecalculatePositions();
}
