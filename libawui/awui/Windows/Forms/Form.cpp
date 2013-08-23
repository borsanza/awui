// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Form.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Diagnostics/Process.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/OpenGL/GL.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Application.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <SDL.h>
#include <SDL_opengl.h>

#ifndef GL_BGRA
	#define GL_BGRA 0x80E1
#endif

using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

Control * Form::controlSelected = NULL;
Bitmap * Form::selectedBitmap = NULL;

Form::Form() {
	this->text = "";
	this->SetBackColor(Drawing::Color::FromArgb(192, 192, 192));

	this->SetBounds(100, 100, 300, 300);
	this->mouseButtons = 0;
	this->mouseControlOver = NULL;
	this->fullscreen = 1;
	this->initialized = 0;
	this->fullscreenWidth = -1;
	this->fullscreenHeight = -1;

	Stats * stats = Stats::Instance();
	stats->SetDock(DockStyle::Bottom);
	this->GetControls()->Add(stats);

	this->remoteProcess = new awui::Diagnostics::Process();
	this->remoteProcess->Start("cat", "/dev/ttyUSB0");
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
	glViewport(0, 0, this->GetWidth(), this->GetHeight());
	GL gl;
	Drawing::Rectangle rectangle;
	rectangle.SetX(0);
	rectangle.SetY(0);
	rectangle.SetWidth(this->GetWidth());
	rectangle.SetHeight(this->GetHeight());
	gl.SetClippingBase(rectangle);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	this->OnPaintPre(0, 0, this->GetWidth(), this->GetHeight(), &gl);

	Stats * stats = Stats::Instance();
	stats->SetTimeBeforeVSync();
}

#include <awui/Console.h>

void Form::ProcessEvents() {
	Stats * stats = Stats::Instance();
	stats->SetTimeAfterVSync();
	int resizex = -1;
	int resizey = -1;
	SDL_Event event;

	if (this->remoteProcess->GetHasString()) {
		awui::String line = this->remoteProcess->GetLine();
		line = line.Substring(0, 4);
		if (line == "12:1")
			OnRemoteKeyPressedPre(RemoteButtons::Right);
		if (line == "13:1")
			OnRemoteKeyPressedPre(RemoteButtons::Left);
		if (line == "14:1")
			OnRemoteKeyPressedPre(RemoteButtons::Up);
		if (line == "15:1")
			OnRemoteKeyPressedPre(RemoteButtons::Down);
		if ((line == "16:1") || (line == "11:1"))
			OnRemoteKeyPressedPre(RemoteButtons::Ok);
		if (line == "10:1")
			OnRemoteKeyPressedPre(RemoteButtons::Menu);
		if (line == "31:1")
			SetFullscreen(!this->fullscreen);
		if (line == "32:1")
			Application::Quit();
	}

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_JOYBUTTONDOWN:
				switch (event.jbutton.button) {
					case 0:
						OnRemoteKeyPressedPre(RemoteButtons::Ok);
						break;
					case 1:
						OnRemoteKeyPressedPre(RemoteButtons::Menu);
						break;
					default:
						break;
				}
				break;
			case SDL_JOYAXISMOTION:
				if (event.jaxis.axis == 0) {
					if (event.jaxis.value < -16000)
							OnRemoteKeyPressedPre(RemoteButtons::Left);
					if (event.jaxis.value > 16000)
							OnRemoteKeyPressedPre(RemoteButtons::Right);
				}

				if (event.jaxis.axis == 1) {
					if (event.jaxis.value < -16000)
							OnRemoteKeyPressedPre(RemoteButtons::Up);
					if (event.jaxis.value > 16000)
							OnRemoteKeyPressedPre(RemoteButtons::Down);
				}
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						if (event.key.keysym.mod & KMOD_LCTRL)
							Application::Quit();
						else
							OnRemoteKeyPressedPre(RemoteButtons::Menu);
						break;
					case SDLK_RETURN:
					case SDLK_KP_ENTER:
						OnRemoteKeyPressedPre(RemoteButtons::Ok);
						break;
					case SDLK_w:
						if (event.key.keysym.mod & KMOD_LALT)
							this->SetFullscreen(0);
						break;
					case SDLK_f:
						if (event.key.keysym.mod & KMOD_LALT)
							this->SetFullscreen(1);
						break;
					case SDLK_LEFT:
						OnRemoteKeyPressedPre(RemoteButtons::Left);
						break;
					case SDLK_RIGHT:
						OnRemoteKeyPressedPre(RemoteButtons::Right);
						break;
					case SDLK_UP:
						OnRemoteKeyPressedPre(RemoteButtons::Up);
						break;
					case SDLK_DOWN:
						OnRemoteKeyPressedPre(RemoteButtons::Down);
						break;
					default:
						break;
				}
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
			case SDL_VIDEORESIZE:
				resizex = event.resize.w;
				resizey = event.resize.h;
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

#include <GL/glx.h>
//#include <GL/glxext.h>
/*
bool checkGLXExtension(const char* extName, SDL_Surface * screen)
{
	char* list = (char*) glXQueryExtensionsString(32, screen);
	char* end;
	int extNameLen;

	extNameLen = strlen(extName);
	end = list + strlen(list);

	while (list < end)
	{
		int n = strcspn(list, " ");

		if ((extNameLen == n) && (strncmp(extName, list, n) == 0))
			return true;

		list += (n + 1);
	};
	return false;
};
*/

void Form::RefreshVideo() {
	if (!initialized)
		return;

	SDL_Surface * screen;

	int flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL;
	int width = 0;
	int height = 0;

	if (this->fullscreenWidth == -1) {
		screen = SDL_SetVideoMode(0, 0, 32, flags | SDL_FULLSCREEN);
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

	screen = SDL_SetVideoMode(width, height, 32, flags);

	void (*swapInterval)(int);
//	if (checkGLXExtension("GLX_MESA_swap_control", screen))
//		swapInterval = (void (*)(int)) glXGetProcAddress((const GLubyte*) "glXSwapIntervalMESA");
//	else if (checkGLXExtension("GLX_SGI_swap_control", screen))
	swapInterval = (void (*)(int)) glXGetProcAddress((const GLubyte*) "glXSwapIntervalSGI");
	swapInterval(1);
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

Control * Form::GetControlSelected() {
	return Form::controlSelected;
}

void Form::SetControlSelected(Control * selected) {
	Form::controlSelected = selected;
	Form::GetSelectedBitmap()->SetParent(selected->GetParent());
	selected->SetFocus();
}

Bitmap * Form::GetSelectedBitmap() {
	if (!Form::selectedBitmap) {
		Bitmap * bitmap = new Bitmap("images/button.png");
		bitmap->SetDock((DockStyle::Enum) 0);//DockStyle::None);
		bitmap->SetBackColor(Drawing::Color::FromArgb(0, 0, 0, 0));
		bitmap->SetFixedMargins(28, 25, 28, 24);
		bitmap->SetLocation(0, 0);
		bitmap->SetSize(Drawing::Size(97, 97));
		Form::selectedBitmap = bitmap;
	}

	return Form::selectedBitmap;
}
