/*
 * awui/Windows/Forms/Form.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Form.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Diagnostics/Process.h>
#include <awui/DateTime.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/IO/Directory.h>
#include <awui/OpenGL/GL.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Application.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Keys.h>
#include <awui/Windows/Forms/RemoteButtons.h>
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
	this->remoteProcess->Start("cat", "/dev/ttyUSB0 2> /dev/null");
}

Form::~Form() {
	delete this->remoteProcess;
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

	int r = this->OnPaintPre(0, 0, this->GetWidth(), this->GetHeight(), &gl, true);

	Stats * stats = Stats::Instance();
	stats->SetDrawedControls(r);
}

void Form::OnRemoteHeartbeat() {
	Stats * stats = Stats::Instance();
	stats->OnRemoteHeartbeat();
}

void Form::OnTick() {
	static DateTime lastTime = DateTime::GetNow();
	DateTime now = DateTime::GetNow();
	if (lastTime.GetSecond() != now.GetSecond()) {
		lastTime = now;
		if (this->remoteProcess->GetHasExited()) {
			delete this->remoteProcess;
			this->remoteProcess = new awui::Diagnostics::Process();
			this->remoteProcess->Start("cat", "/dev/ttyUSB0 2> /dev/null");
		}
	}
}

void Form::ProcessEvents() {
	int resizex = -1;
	int resizey = -1;
	SDL_Event event;

	if (this->remoteProcess->GetHasString()) {
		awui::String line = this->remoteProcess->GetLine();

		line = line.Substring(0, 4);
		if ((line == "lub ") || (line == "dub "))
			OnRemoteHeartbeat();

		if (line == "12:1")
			OnRemoteKeyPressPre(RemoteButtons::Right);
		if (line == "13:1")
			OnRemoteKeyPressPre(RemoteButtons::Left);
		if (line == "14:1")
			OnRemoteKeyPressPre(RemoteButtons::Up);
		if (line == "15:1")
			OnRemoteKeyPressPre(RemoteButtons::Down);
		if ((line == "16:1") || (line == "11:1"))
			OnRemoteKeyPressPre(RemoteButtons::Ok);
		if (line == "10:1")
			OnRemoteKeyPressPre(RemoteButtons::Menu);
		if (line == "31:1")
			SetFullscreen(!this->fullscreen);
		if (line == "32:1")
			Application::Quit();

		if (line == "12:0")
			OnRemoteKeyUpPre(RemoteButtons::Right);
		if (line == "13:0")
			OnRemoteKeyUpPre(RemoteButtons::Left);
		if (line == "14:0")
			OnRemoteKeyUpPre(RemoteButtons::Up);
		if (line == "15:0")
			OnRemoteKeyUpPre(RemoteButtons::Down);
		if ((line == "16:0") || (line == "11:0"))
			OnRemoteKeyUpPre(RemoteButtons::Ok);
		if (line == "10:0")
			OnRemoteKeyUpPre(RemoteButtons::Menu);
		if (line == "32:0")
			Application::Quit();
	}

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_JOYBUTTONDOWN:
				switch (event.jbutton.button) {
					case 0:
						OnRemoteKeyPressPre(RemoteButtons::Ok);
						break;
					case 1:
						OnRemoteKeyPressPre(RemoteButtons::Menu);
						break;
					default:
						break;
				}
				break;

			case SDL_JOYAXISMOTION:
				if (event.jaxis.axis == 0) {
					if (event.jaxis.value < -16000)
							OnRemoteKeyPressPre(RemoteButtons::Left);
					if (event.jaxis.value > 16000)
							OnRemoteKeyPressPre(RemoteButtons::Right);
				}

				if (event.jaxis.axis == 1) {
					if (event.jaxis.value < -16000)
							OnRemoteKeyPressPre(RemoteButtons::Up);
					if (event.jaxis.value > 16000)
							OnRemoteKeyPressPre(RemoteButtons::Down);
				}
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						if (event.key.keysym.mod & KMOD_LCTRL)
							Application::Quit();
						else
							OnRemoteKeyPressPre(RemoteButtons::Menu);
						break;
					case SDLK_RETURN:
					case SDLK_KP_ENTER:
						OnRemoteKeyPressPre(RemoteButtons::Ok);
						break;
					case SDLK_LEFT:
						OnRemoteKeyPressPre(RemoteButtons::Left);
						break;
					case SDLK_RIGHT:
						OnRemoteKeyPressPre(RemoteButtons::Right);
						break;
					case SDLK_UP:
						OnRemoteKeyPressPre(RemoteButtons::Up);
						break;
					case SDLK_DOWN:
						OnRemoteKeyPressPre(RemoteButtons::Down);
						break;

					case SDLK_0: OnKeyPressPre(Keys::Key_0); break;
					case SDLK_1: OnKeyPressPre(Keys::Key_1); break;
					case SDLK_2: OnKeyPressPre(Keys::Key_2); break;
					case SDLK_3: OnKeyPressPre(Keys::Key_3); break;
					case SDLK_4: OnKeyPressPre(Keys::Key_4); break;
					case SDLK_5: OnKeyPressPre(Keys::Key_5); break;
					case SDLK_6: OnKeyPressPre(Keys::Key_6); break;
					case SDLK_7: OnKeyPressPre(Keys::Key_7); break;
					case SDLK_8: OnKeyPressPre(Keys::Key_8); break;
					case SDLK_9: OnKeyPressPre(Keys::Key_9); break;
					case SDLK_a: OnKeyPressPre(Keys::Key_A); break;
					case SDLK_b: OnKeyPressPre(Keys::Key_B); break;
					case SDLK_c: OnKeyPressPre(Keys::Key_C); break;
					case SDLK_d: OnKeyPressPre(Keys::Key_D); break;
					case SDLK_e: OnKeyPressPre(Keys::Key_E); break;
					case SDLK_f:
						if (event.key.keysym.mod & KMOD_LALT)
							this->SetFullscreen(1);
						else
							OnKeyPressPre(Keys::Key_F);
						break;

					case SDLK_g: OnKeyPressPre(Keys::Key_G); break;
					case SDLK_h: OnKeyPressPre(Keys::Key_H); break;
					case SDLK_i: OnKeyPressPre(Keys::Key_I); break;
					case SDLK_j: OnKeyPressPre(Keys::Key_J); break;
					case SDLK_k: OnKeyPressPre(Keys::Key_K); break;
					case SDLK_l: OnKeyPressPre(Keys::Key_L); break;
					case SDLK_m: OnKeyPressPre(Keys::Key_M); break;
					case SDLK_n: OnKeyPressPre(Keys::Key_N); break;
					case SDLK_o: OnKeyPressPre(Keys::Key_O); break;
					case SDLK_p: OnKeyPressPre(Keys::Key_P); break;
					case SDLK_q: OnKeyPressPre(Keys::Key_Q); break;
					case SDLK_r: OnKeyPressPre(Keys::Key_R); break;
					case SDLK_s: OnKeyPressPre(Keys::Key_S); break;
					case SDLK_t: OnKeyPressPre(Keys::Key_T); break;
					case SDLK_u: OnKeyPressPre(Keys::Key_U); break;
					case SDLK_v: OnKeyPressPre(Keys::Key_V); break;
					case SDLK_w:
						if (event.key.keysym.mod & KMOD_LALT)
							this->SetFullscreen(0);
						else
							OnKeyPressPre(Keys::Key_W);
						break;

					case SDLK_x: OnKeyPressPre(Keys::Key_X); break;
					case SDLK_y: OnKeyPressPre(Keys::Key_Y); break;
					case SDLK_z: OnKeyPressPre(Keys::Key_Z); break;

					case SDLK_KP0: OnKeyPressPre(Keys::Key_KP0); break;
					case SDLK_KP1: OnKeyPressPre(Keys::Key_KP1); break;
					case SDLK_KP2: OnKeyPressPre(Keys::Key_KP2); break;
					case SDLK_KP3: OnKeyPressPre(Keys::Key_KP3); break;
					case SDLK_KP4: OnKeyPressPre(Keys::Key_KP4); break;
					case SDLK_KP5: OnKeyPressPre(Keys::Key_KP5); break;
					case SDLK_KP6: OnKeyPressPre(Keys::Key_KP6); break;
					case SDLK_KP7: OnKeyPressPre(Keys::Key_KP7); break;
					case SDLK_KP8: OnKeyPressPre(Keys::Key_KP8); break;
					case SDLK_KP9: OnKeyPressPre(Keys::Key_KP9); break;
					case SDLK_KP_DIVIDE: OnKeyPressPre(Keys::Key_KP_DIVIDE); break;
					case SDLK_KP_EQUALS: OnKeyPressPre(Keys::Key_KP_EQUALS); break;
					case SDLK_KP_MINUS: OnKeyPressPre(Keys::Key_KP_MINUS); break;
					case SDLK_KP_MULTIPLY: OnKeyPressPre(Keys::Key_KP_MULTIPLY); break;
					case SDLK_KP_PERIOD: OnKeyPressPre(Keys::Key_KP_PERIOD); break;
					case SDLK_KP_PLUS: OnKeyPressPre(Keys::Key_KP_PLUS); break;

					default:
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_0: OnKeyUpPre(Keys::Key_0); break;
					case SDLK_1: OnKeyUpPre(Keys::Key_1); break;
					case SDLK_2: OnKeyUpPre(Keys::Key_2); break;
					case SDLK_3: OnKeyUpPre(Keys::Key_3); break;
					case SDLK_4: OnKeyUpPre(Keys::Key_4); break;
					case SDLK_5: OnKeyUpPre(Keys::Key_5); break;
					case SDLK_6: OnKeyUpPre(Keys::Key_6); break;
					case SDLK_7: OnKeyUpPre(Keys::Key_7); break;
					case SDLK_8: OnKeyUpPre(Keys::Key_8); break;
					case SDLK_9: OnKeyUpPre(Keys::Key_9); break;
					case SDLK_a: OnKeyUpPre(Keys::Key_A); break;
					case SDLK_b: OnKeyUpPre(Keys::Key_B); break;
					case SDLK_c: OnKeyUpPre(Keys::Key_C); break;
					case SDLK_d: OnKeyUpPre(Keys::Key_D); break;
					case SDLK_e: OnKeyUpPre(Keys::Key_E); break;
					case SDLK_f: OnKeyUpPre(Keys::Key_F); break;
					case SDLK_g: OnKeyUpPre(Keys::Key_G); break;
					case SDLK_h: OnKeyUpPre(Keys::Key_H); break;
					case SDLK_i: OnKeyUpPre(Keys::Key_I); break;
					case SDLK_j: OnKeyUpPre(Keys::Key_J); break;
					case SDLK_k: OnKeyUpPre(Keys::Key_K); break;
					case SDLK_l: OnKeyUpPre(Keys::Key_L); break;
					case SDLK_m: OnKeyUpPre(Keys::Key_M); break;
					case SDLK_n: OnKeyUpPre(Keys::Key_N); break;
					case SDLK_o: OnKeyUpPre(Keys::Key_O); break;
					case SDLK_p: OnKeyUpPre(Keys::Key_P); break;
					case SDLK_q: OnKeyUpPre(Keys::Key_Q); break;
					case SDLK_r: OnKeyUpPre(Keys::Key_R); break;
					case SDLK_s: OnKeyUpPre(Keys::Key_S); break;
					case SDLK_t: OnKeyUpPre(Keys::Key_T); break;
					case SDLK_u: OnKeyUpPre(Keys::Key_U); break;
					case SDLK_v: OnKeyUpPre(Keys::Key_V); break;
					case SDLK_w: OnKeyUpPre(Keys::Key_W); break;
					case SDLK_x: OnKeyUpPre(Keys::Key_X); break;
					case SDLK_y: OnKeyUpPre(Keys::Key_Y); break;
					case SDLK_z: OnKeyUpPre(Keys::Key_Z); break;

					case SDLK_KP0: OnKeyUpPre(Keys::Key_KP0); break;
					case SDLK_KP1: OnKeyUpPre(Keys::Key_KP1); break;
					case SDLK_KP2: OnKeyUpPre(Keys::Key_KP2); break;
					case SDLK_KP3: OnKeyUpPre(Keys::Key_KP3); break;
					case SDLK_KP4: OnKeyUpPre(Keys::Key_KP4); break;
					case SDLK_KP5: OnKeyUpPre(Keys::Key_KP5); break;
					case SDLK_KP6: OnKeyUpPre(Keys::Key_KP6); break;
					case SDLK_KP7: OnKeyUpPre(Keys::Key_KP7); break;
					case SDLK_KP8: OnKeyUpPre(Keys::Key_KP8); break;
					case SDLK_KP9: OnKeyUpPre(Keys::Key_KP9); break;
					case SDLK_KP_DIVIDE: OnKeyUpPre(Keys::Key_KP_DIVIDE); break;
					case SDLK_KP_ENTER: OnKeyUpPre(Keys::Key_KP_ENTER); break;
					case SDLK_KP_EQUALS: OnKeyUpPre(Keys::Key_KP_EQUALS); break;
					case SDLK_KP_MINUS: OnKeyUpPre(Keys::Key_KP_MINUS); break;
					case SDLK_KP_MULTIPLY: OnKeyUpPre(Keys::Key_KP_MULTIPLY); break;
					case SDLK_KP_PERIOD: OnKeyUpPre(Keys::Key_KP_PERIOD); break;
					case SDLK_KP_PLUS: OnKeyUpPre(Keys::Key_KP_PLUS); break;

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

void Form::RefreshVideo() {
	if (!initialized)
		return;

	int flags = SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL;
	int width = 0;
	int height = 0;

	if (this->fullscreenWidth == -1) {
		const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo();
		this->fullscreenWidth = videoInfo->current_w;
		this->fullscreenHeight = videoInfo->current_h;
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

Control * Form::GetControlSelected() {
	return Form::controlSelected;
}

void Form::SetControlSelected(Control * selected) {
	if (!selected || selected->GetTabStop()) {
		Form::controlSelected = selected;
		if (selected) {
			Form::GetSelectedBitmap()->SetParent(selected->GetParent());
			selected->SetFocus(false);
		}
	}
}

Bitmap * Form::GetSelectedBitmap() {
	if (!Form::selectedBitmap) {
		String file = IO::Directory::GetWorkingDirectory();
		Bitmap * bitmap = new Bitmap(file + "/images/button.png");
		bitmap->SetDock((DockStyle::Enum) 0);//DockStyle::None);
		bitmap->SetBackColor(Drawing::Color::FromArgb(0, 0, 0, 0));
		bitmap->SetFixedMargins(28, 25, 28, 24);
		bitmap->SetLocation(0, 0);
		bitmap->SetSize(Drawing::Size(97, 97));
		Form::selectedBitmap = bitmap;
	}

	return Form::selectedBitmap;
}
