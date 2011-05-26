// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiGL.h"

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

awuiGL::awuiGL() {
}

void awuiGL::SetClippingBase(awuiRectangle rectangle) {
	this->rectangle1 = rectangle;
}

void awuiGL::SetClipping(awuiRectangle rectangle) {
	this->rectangle2 = rectangle;
}

awuiRectangle awuiGL::GetClippingBase() {
	return this->rectangle1;
}

awuiRectangle awuiGL::GetClipping() {
	return this->rectangle2;
}

awuiRectangle awuiGL::GetClippingResult() {
	awuiRectangle rect = this->rectangle1;

	rect.Intersect(this->rectangle2);
	
	return rect; 
}

void awuiGL::SetClipping() {
	awuiRectangle rect = this->GetClippingResult();

	glScissor(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
}