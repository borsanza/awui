// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiButton.h"
#include "awuiColor.h"

awuiButton::awuiButton() {
	this->SetSize(75,23);
	awuiColor * color = awuiColor::FromArgb(255, 0, 0);
	this->SetBackColor(color);
	delete color;
}

awuiButton::~awuiButton() {
}