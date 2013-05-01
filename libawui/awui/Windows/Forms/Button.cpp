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
	this->SetBackColor(Color::FromArgb(0, 0, 0));
	this->testx = 0;
	this->testy = 0;
	this->show = 0;
	this->image = NULL;
	this->g = NULL;
}

Button::~Button() {
	if (this->g)
		delete this->g;

	if (this->image)
		delete this->image;
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
	if (this->show) {
		glColor3f(0.3f, 0.3f, 1.0f);
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(this->testx, this->testy);
		glVertex2f(this->GetWidth() - 1, 0);
		glVertex2f(this->testx, this->testy);
		glVertex2f(0, this->GetHeight() - 1);
		glVertex2f(this->testx, this->testy);
		glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
		glVertex2f(this->testx, this->testy);
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

	if (image) {
		float posY;
		posY = this->GetHeight() / 2.0f;
		posY += this->metrics.GetBearingY();
		posY += 2;
//		posY += (this->metrics.GetHeight() + this->metrics.GetBearingY()) / 2.0f;
		GL::DrawImageGL(image, Math::Round((this->GetWidth() - this->metrics.GetWidth()) / 2.0f), Math::Round(posY));
	}
}

void Button::SetText(const String str) {
	this->text = str;

	if (this->g)
		delete this->g;

	if (this->image)
		delete this->image;

	Font font = Font("Monospace", 20, FontStyle::Bold); // FontStyle::Strikeout | FontStyle::Underline);
	this->metrics = TextRenderer::GetMeasureText(this->text, &font);

	this->image = new Drawing::Image(this->metrics.GetWidth(), this->metrics.GetHeight());
	this->g = Drawing::Graphics::FromImage(this->image);
	this->g->DrawString(this->text, &font, Color::FromArgb(255, 255, 255), 0, 0);
}

const awui::String Button::GetName() {
	return this->text;
}
