// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "GL.h"

#include <awui/Drawing/Image.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;

GL::GL() {
}

void GL::SetClippingBase(Rectangle rectangle) {
	this->rectangle1 = rectangle;
}

void GL::SetClipping(Rectangle rectangle) {
	this->rectangle2 = rectangle;
}

Rectangle GL::GetClippingBase() {
	return this->rectangle1;
}

Rectangle GL::GetClipping() {
	return this->rectangle2;
}

Rectangle GL::GetClippingResult() {
	return Rectangle::Intersect(this->rectangle1, this->rectangle2);
}

void GL::SetClipping() {
	Rectangle rect = this->GetClippingResult();

	glScissor(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
}

void GL::DrawImageGL(awui::Drawing::Image * image, float x, float y) {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelZoom(1.0f, -1.0f);
	glRasterPos2f(x, y);
	glDrawPixels(image->GetWidth(), image->GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, image->image);
	glDisable(GL_BLEND);
}
