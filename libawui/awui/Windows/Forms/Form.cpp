// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Form.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Application.h>
#include <SDL.h>
#include <SDL_opengl.h>

#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
#endif

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Form::Form() {
	this->text = "";
	this->SetBackColor(Color::FromArgb(192, 192, 192));

	this->SetBounds(100, 100, 300, 300);
	this->mouseButtons = 0;
	this->mouseControlOver = NULL;
	this->fullscreen = 1;
	this->initialized = 0;
	this->fullscreenWidth = -1;
	this->fullscreenHeight = -1;
}

Form::~Form() {
}

int Form::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Form)
		return 1;

	return Control::IsClass(objectClass);
}

void Form::Init() {
	this->initialized = 1;
	this->RefreshVideo();
	this->SetText(this->text);
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

void Form::ProcessEvents() {
	int resizex = -1;
	int resizey = -1;
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_VIDEORESIZE:
				resizex = event.resize.w;
				resizey = event.resize.h;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					Application::Quit();
				if (event.key.keysym.sym == SDLK_w)
					this->SetFullscreen(0);
				if (event.key.keysym.sym == SDLK_f)
					this->SetFullscreen(1);
				break;
			case SDL_QUIT:
				Application::Quit();
				break;
			case SDL_MOUSEBUTTONDOWN: {
					MouseButtons::Enum button = MouseButtons::None;
					switch (event.button.button) {
						case SDL_BUTTON_WHEELUP:
							button = MouseButtons::XButton1;
							break;
						case SDL_BUTTON_WHEELDOWN:
							button = MouseButtons::XButton2;
							break;
						case SDL_BUTTON_LEFT:
							button = MouseButtons::Left;
							break;
						case SDL_BUTTON_RIGHT:
							button = MouseButtons::Right;
							break;
						case SDL_BUTTON_MIDDLE:
							button = MouseButtons::Middle;
							break;
					}
					if (button) {
						this->mouseButtons |= button;
						this->OnMouseDownPre(this->mouseX, this->mouseY, button, this->mouseButtons);
					}
				}
				break;
			case SDL_MOUSEBUTTONUP: {
					MouseButtons::Enum button = MouseButtons::None;
					switch (event.button.button) {
						case SDL_BUTTON_WHEELUP:
							button = MouseButtons::XButton1;
							break;
						case SDL_BUTTON_WHEELDOWN:
							button = MouseButtons::XButton2;
							break;
						case SDL_BUTTON_LEFT:
							button = MouseButtons::Left;
							break;
						case SDL_BUTTON_RIGHT:
							button = MouseButtons::Right;
							break;
						case SDL_BUTTON_MIDDLE:
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
			case SDL_MOUSEMOTION:
				this->mouseX = event.motion.x;
				this->mouseY = event.motion.y;
				this->OnMouseMovePre(this->mouseX, this->mouseY, this->mouseButtons);
				break;
			default:
				break;
		}
	}

	if ((resizex != -1) && (resizey != -1)) {
		this->SetSize(resizex, resizey);
		this->RefreshVideo();
	}
}

void Form::RefreshVideo() {
	if (!initialized)
		return;

	int flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL;
	int width = 0;
	int height = 0;

	if (this->fullscreenWidth == -1) {
		SDL_Surface * screen = SDL_SetVideoMode(0, 0, 32, flags | SDL_FULLSCREEN);
		this->fullscreenWidth = screen->w;
		this->fullscreenHeight = screen->h;
	}

	if (!this->fullscreen) {
		width = this->GetWidth();
		height = this->GetHeight();
 		flags |= SDL_RESIZABLE;
	} else {
		width = this->fullscreenWidth;
		height = this->fullscreenHeight;
 		flags |= SDL_FULLSCREEN;
		this->SetSize(width, height);
	}

	SDL_SetVideoMode(width, height, 32, flags);
}

void Form::SetFullscreen(int mode) {
	if (this->fullscreen == mode)
		return;

	this->fullscreen = mode;
	this->RefreshVideo();
}

void Form::SetText(String title) {
	this->text = title;

	if (initialized)
		SDL_WM_SetCaption(this->text.ToCharArray(), NULL);
}
