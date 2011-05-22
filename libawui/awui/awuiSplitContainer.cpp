// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitContainer.h"

#include "awuiArrayList.h"
#include "awuiColor.h"
#include "awuiPanel.h"
#include "awuiSplitter.h"

awuiSplitContainer::awuiSplitContainer() {
	this->splitterDistance = 100;
	this->splitterIncrement = 1;
	this->splitterWidth = 4;
	this->orientation = Vertical;
	
	this->SetName("awuiSplitContainer");
	
  awuiColor * color = awuiColor::FromArgb(255, 255, 0);
  this->SetBackColor(color);
  delete color;
	
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

int awuiSplitContainer::IsClass(awuiObject::awuiClasses objectClass) {
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

void awuiSplitContainer::RecalculatePositions() {
	int x2 = this->splitterWidth >> 1;
	int x1 = this->splitterWidth - x2;

	this->panel1->SetBounds(0, 0, this->splitterDistance - x1, this->GetHeight());
	this->splitter->SetBounds(this->splitterDistance - x1, 0, this->splitterWidth, this->GetHeight());
	this->panel2->SetBounds(this->splitterDistance + x2, 0, this->GetWidth() - (this->splitterDistance + x2), this->GetHeight());
}

float awuiSplitContainer::GetSplitterDistance() {
	return this->splitterDistance;
}

void awuiSplitContainer::SetSplitterDistance(int distance) {
	this->splitterDistance = distance;
}

int awuiSplitContainer::GetSplitterIncrement() {
	return this->splitterIncrement;
}

void awuiSplitContainer::SetSplitterIncrement(int increment) {
	this->splitterIncrement = increment;
}

int awuiSplitContainer::GetSplitterWidth() {
	return this->splitterWidth;
}

void awuiSplitContainer::SetSplitterWidth(int width) {
	this->splitterWidth = width;
}

awuiSplitContainer::Orientation awuiSplitContainer::GetOrientation() {
	return this->orientation;
}

void awuiSplitContainer::SetOrientation(Orientation orientation) {
	this->orientation = orientation;
}

void awuiSplitContainer::Layout() {
	this->RecalculatePositions();
}