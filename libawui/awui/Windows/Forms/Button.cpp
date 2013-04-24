// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Button.h"

#include <awui/Math.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/TextRenderer.h>
#include <SDL_opengl.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Button::Button() {
	this->SetSize(75,23);
	this->SetBackColor(Color::FromArgb(255, 255, 255));
	this->testx = 0;
	this->testy = 0;
	this->show = 0;
}

Button::~Button() {
}

int Button::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Button)
		return 1;

	return Control::IsClass(objectClass);
}

void Button::OnMouseLeave() {
	this->testx = -1;
	this->testy = -1;
	this->show = 0;
}

void Button::OnMouseDown(MouseEventArgs* e) {
	this->show = 1;
	this->testx = e->GetX();
	this->testy = e->GetY();
}

void Button::OnMouseMove(MouseEventArgs* e) {
	this->show = 1;
	this->testx = e->GetX();
	this->testy = e->GetY();
}

void Button::OnPaint(GL* gl) {
	Font font = Font("Monospace", 11);

	Size size = TextRenderer::GetMeasureText(this->text, &font);

	Drawing::Image * image = new Drawing::Image(size.GetWidth(), size.GetHeight());
	Drawing::Graphics * g = Drawing::Graphics::FromImage(image);
	g->DrawString(this->text, &font, Color::FromArgb(0, 0, 0), 0, 0);

	GL::DrawImageGL(image, Math::Round((this->GetWidth() - size.GetWidth()) / 2.0f), Math::Round((this->GetHeight() - size.GetHeight()) / 2.0f));

	delete g;
	delete image;

//	std::cout << size.GetWidth() << "x" << size.GetHeight() << ": " << this->text << std::endl;

	if (!this->show)
		return;

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(testx, this->testy);
	glVertex2f(this->GetWidth() - 1, 0);
	glVertex2f(testx, this->testy);
	glVertex2f(0, this->GetHeight() - 1);
	glVertex2f(testx, this->testy);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(testx, this->testy);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(0, this->GetHeight() - 1);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(this->GetWidth() - 1, 0);
	glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
	glVertex2f(0, 0);
	glVertex2f(this->GetWidth() - 1, 0);
	glVertex2f(0, 0);
	glVertex2f(0, this->GetHeight() - 1);
	glEnd();
}

void Button::SetText(const String str) {
	this->text = str;
}

const awui::String Button::GetName() {
	return this->text;
}
