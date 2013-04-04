// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitContainer.h"

#include "awuiControlCollection.h"
#include "awuiColor.h"
#include "awuiPanel.h"
#include "awuiSplitter.h"

#include <iostream>

awuiSplitContainer::awuiSplitContainer() {
    this->splitterDistance = 100;
    this->splitterIncrement = 1;
    this->splitterWidth = 4;
    this->orientation = Vertical;

    this->SetName("awuiSplitContainer");

    this->SetBackColor(awuiColor::FromArgb(255, 255, 0));

    this->panel1 = new awuiPanel();
    this->splitter = new awuiSplitter();
    this->splitter->SetOrientation(this->orientation);
    this->panel2 = new awuiPanel();
    this->GetControls()->Add(this->panel1);
    this->GetControls()->Add(this->splitter);
    this->GetControls()->Add(this->panel2);

    this->SetSize(200, 200);
}

awuiSplitContainer::~awuiSplitContainer() {
    delete this->panel1;
    delete this->splitter;
    delete this->panel2;
}

int awuiSplitContainer::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::SplitContainer)
		return 1;

	return awuiControl::IsClass(objectClass);
}

awuiPanel * awuiSplitContainer::GetPanel1() {
	return this->panel1;
}

awuiPanel * awuiSplitContainer::GetPanel2() {
	return this->panel2;
}

void awuiSplitContainer::SetLocationW(awuiControl * control, int pos) {
	if (this->orientation == awuiSplitContainer::Vertical)
		control->SetLocation(pos, 0);
	else
		control->SetLocation(0, pos);
}

void awuiSplitContainer::SetSizeW(awuiControl * control, int size, int substract) {
	if (this->orientation == awuiSplitContainer::Vertical) {
		if (substract)
			size = this->GetWidth() - size;

		control->SetSize(size, this->GetHeight());
	} else {
		if (substract)
			size = this->GetHeight() - size;

		control->SetSize(this->GetWidth(), size);
	}
}

int awuiSplitContainer::GetSizeW(awuiControl * control) const {
	if (this->orientation == awuiSplitContainer::Vertical)
		return control->GetWidth();
	else
		return control->GetHeight();
}

int awuiSplitContainer::GetMinimumSizeW(awuiControl * control) const {
	if (this->orientation == awuiSplitContainer::Vertical)
		return control->GetMinimumSize().GetWidth();
	else
		return control->GetMinimumSize().GetHeight();
}

void awuiSplitContainer::RecalculatePositions() {
    int distance = this->splitterDistance;
	int x2 = this->splitterWidth >> 1;
	int x1 = this->splitterWidth - x2;

	int size = (this->orientation == awuiSplitContainer::Vertical)? this->GetWidth() : this->GetHeight();

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

float awuiSplitContainer::GetSplitterDistance() const {
	return this->splitterDistance;
}

void awuiSplitContainer::SetSplitterDistance(int distance) {
	this->splitterDistance = distance;
	this->RecalculatePositions();
}

int awuiSplitContainer::GetSplitterIncrement() const {
	return this->splitterIncrement;
}

void awuiSplitContainer::SetSplitterIncrement(int increment) {
	this->splitterIncrement = increment;
}

int awuiSplitContainer::GetSplitterWidth() const {
	return this->splitterWidth;
}

void awuiSplitContainer::SetSplitterWidth(int width) {
	this->splitterWidth = width;
}

awuiSplitContainer::Orientation awuiSplitContainer::GetOrientation() const {
	return this->orientation;
}

void awuiSplitContainer::SetOrientation(Orientation orientation) {
	this->orientation = orientation;
	this->splitter->SetOrientation(orientation);
	this->RecalculatePositions();
}

void awuiSplitContainer::Layout() {
	this->RecalculatePositions();
}
