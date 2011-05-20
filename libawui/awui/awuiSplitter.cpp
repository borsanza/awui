// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitter.h"

#include "awuiSplitContainer.h"
#include "awuiColor.h"

awuiSplitter::awuiSplitter() {
  awuiColor * color = awuiColor::FromArgb(0, 255, 0);
  this->SetBackColor(color);
  delete color;
	
	this->SetName("awuiSplitter");

	this->SetSize(20, 200);
}

awuiSplitter::~awuiSplitter() {
}

awuiSplitContainer::Orientation awuiSplitter::GetOrientation() {
	return this->orientation;
}

void awuiSplitter::SetOrientation(awuiSplitContainer::Orientation orientation) {
	this->orientation = orientation;
}