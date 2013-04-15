// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <stdarg.h>

#include "Form.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Application.h>

extern "C" {
	#include <aw/sysgl.h>
	#include <aw/aw.h>
}

#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
#endif

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Form::Form() {
	this->SetBackColor(Color::FromArgb(192, 192, 192));

	this->SetBounds(100, 100, 300, 300);
	this->mouseButtons = 0;
	this->mouseControlOver = NULL;

	glGenTextures(0, &this->texture1);
	glGenTextures(1, &this->texture2);

	this->old1w = -1;
	this->old1h = -1;
	this->old2w = -1;
	this->old2h = -1;
}

Form::~Form() {
	glDeleteTextures(1, &this->texture2);
	glDeleteTextures(0, &this->texture1);

	awMakeCurrent(this->w, NULL);
	awDel(this->w);
}

int Form::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Form)
		return 1;

	return Control::IsClass(objectClass);
}

void Form::Show() {
	this->w = awNew(Application::g);
	awGeometry(this->w, this->GetLeft(), this->GetTop(), this->GetWidth(), this->GetHeight());
	awShow(this->w);
//	awHideBorders(this->w);
//	awMaximize(this->w);
}

void Form::OnPaintForm() {
	this->OnMouseMovePre(this->mouseX, this->mouseY, this->mouseButtons);
	glViewport(0, 0, this->GetWidth(), this->GetHeight());
	glEnable(GL_SCISSOR_TEST);

	GL gl;
	Rectangle rectangle;
	rectangle.SetX(0);
	rectangle.SetY(0);
	rectangle.SetWidth(this->GetWidth());
	rectangle.SetHeight(this->GetHeight());
	gl.SetClippingBase(rectangle);
	this->OnPaintPre(0, 0, this->GetWidth(), this->GetHeight(), &gl);
}

void Form::ProcessEvents(ac * c) {
	const ae * e;
	aw * w = this->w;

	int resizex = -1;
	int resizey = -1;

	while ((e = awNextEvent(w))) {
		switch (aeType(e)) {
			case AW_EVENT_RESIZE:
				resizex = aeWidth(e);
				resizey = aeHeight(e);
				break;
			case AW_EVENT_DOWN: {
					MouseButtons::Enum button = MouseButtons::None;
					switch (aeWhich(e)) {
						case AW_KEY_MOUSEWHEELUP:
							button = MouseButtons::XButton1;
							break;
						case AW_KEY_MOUSEWHEELDOWN:
							button = MouseButtons::XButton2;
							break;
						case AW_KEY_MOUSELEFT:
							button = MouseButtons::Left;
							break;
						case AW_KEY_MOUSERIGHT:
							button = MouseButtons::Right;
							break;
						case AW_KEY_MOUSEMIDDLE:
							button = MouseButtons::Middle;
							break;
						default:
							break;
					}

					if (button) {
						this->mouseButtons |= button;
						this->OnMouseDownPre(this->mouseX, this->mouseY, button, this->mouseButtons);
					}
				}
				break;
			case AW_EVENT_UP: {
					MouseButtons::Enum button = MouseButtons::None;
					switch (aeWhich(e)) {
						case AW_KEY_MOUSEWHEELUP:
							button = MouseButtons::XButton1;
							break;
						case AW_KEY_MOUSEWHEELDOWN:
							button = MouseButtons::XButton2;
							break;
						case AW_KEY_MOUSELEFT:
							button = MouseButtons::Left;
							break;
						case AW_KEY_MOUSERIGHT:
							button = MouseButtons::Right;
							break;
						case AW_KEY_MOUSEMIDDLE:
							button = MouseButtons::Middle;
							break;
						default:
							break;
					}

					if (button) {
						this->mouseButtons &= ~button;
						this->OnMouseUpPre(button, this->mouseButtons);
					}
				}
				break;
/*
			case AW_EVENT_CLOSE:
				g_exit = 1;
				break;
			case AW_EVENT_UNICODE:
				Log("Unicode: %s", awKeyName(awe->u.unicode.which));
				break;
*/
			case AW_EVENT_MOTION:
				this->mouseX = aeX(e);
				this->mouseY = aeY(e);
				this->OnMouseMovePre(this->mouseX, this->mouseY, this->mouseButtons);
				break;
			default:
				break;
		}
	}

	if ((resizex != -1) && (resizey != -1)) {
		this->SetSize(resizex, resizey);
//		this->OnResizePre();
	}
}
