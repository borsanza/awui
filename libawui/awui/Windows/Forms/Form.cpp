/**
 * awui/Windows/Forms/Form.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Form.h"

#include <awui/Diagnostics/Process.h>
#include <awui/Drawing/Image.h>
#include <awui/IO/Directory.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Application.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Statistics/Stats.h>

#include <SDL.h>
#include <SDL_opengl.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

Control * Form::controlSelected = NULL;
Bitmap * Form::selectedBitmap = NULL;
uint32_t Form::_buttonsPad1 = 0;
uint32_t Form::_buttonsPad2 = 0;

Form::Form() {
	this->window = 0;
	this->context = 0;
	this->mouseX = 0;
	this->mouseY = 0;
	this->text = "";
	this->SetBackColor(Drawing::Color::FromArgb(192, 192, 192));

	this->SetBounds(100, 100, 300, 300);
	this->mouseButtons = 0;
	this->mouseControlOver = NULL;
	this->initialized = 0;

	this->lastFullscreenState = -1;
	this->fullscreen = 1;
	this->lastWidth = 0;
	this->lastHeight = 0;

	Stats * stats = Stats::Instance();
	stats->SetDock(DockStyle::None);
	this->GetControls()->Add(stats);

//	this->remoteProcess = new awui::Diagnostics::Process();
//	this->remoteProcess->Start("cat", "/dev/ttyUSB0 2> /dev/null");
}

Form::~Form() {
	if (this->context)
		SDL_GL_DeleteContext(this->context);

	if (this->window)
		SDL_DestroyWindow(this->window);

//	delete this->remoteProcess;
}

bool Form::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Form) {
		return true;
	}

	return Control::IsClass(objectClass);
}

void Form::Init() {
	this->initialized = 1;
	this->RefreshVideo();
	this->SetText(this->text);
}

void Form::OnPaintForm() {
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
	Stats * stats = Stats::Instance();
	this->GetControls()->MoveToEnd(stats);
	stats->SetWidth(this->GetWidth());
	stats->SetLocation(0, this->GetHeight() - stats->GetHeight());

	static DateTime lastTime = DateTime::GetNow();
	DateTime now = DateTime::GetNow();
	if (lastTime.GetSecond() != now.GetSecond()) {
		lastTime = now;
/*
		if (this->remoteProcess->GetHasExited()) {
			delete this->remoteProcess;
			this->remoteProcess = new awui::Diagnostics::Process();
			this->remoteProcess->Start("cat", "/dev/ttyUSB0 2> /dev/null");
		}
*/
	}
}

void Form::ProcessEvents() {
	int resizex = -1;
	int resizey = -1;
	SDL_Event event;
/*
	if (this->remoteProcess->GetHasString()) {
		awui::String line = this->remoteProcess->GetLine();

		line = line.Substring(0, 4);
		if ((line == "lub ") || (line == "dub "))
			OnRemoteHeartbeat();

		if (line == "12:1")
			OnRemoteKeyPressPre(0, RemoteButtons::Right);
		if (line == "13:1")
			OnRemoteKeyPressPre(0, RemoteButtons::Left);
		if (line == "14:1")
			OnRemoteKeyPressPre(0, RemoteButtons::Up);
		if (line == "15:1")
			OnRemoteKeyPressPre(0, RemoteButtons::Down);
		if ((line == "16:1") || (line == "11:1"))
			OnRemoteKeyPressPre(0, RemoteButtons::Ok);
		if (line == "17:1")
			OnRemoteKeyPressPre(0, RemoteButtons::Play);
		if (line == "10:1")
			OnRemoteKeyPressPre(0, RemoteButtons::Menu);
		if (line == "31:1")
			SetFullscreen(!this->fullscreen);
		if (line == "32:1")
			Application::Quit();

		if (line == "12:0")
			OnRemoteKeyUpPre(0, RemoteButtons::Right);
		if (line == "13:0")
			OnRemoteKeyUpPre(0, RemoteButtons::Left);
		if (line == "14:0")
			OnRemoteKeyUpPre(0, RemoteButtons::Up);
		if (line == "15:0")
			OnRemoteKeyUpPre(0, RemoteButtons::Down);
		if ((line == "16:0") || (line == "11:0"))
			OnRemoteKeyUpPre(0, RemoteButtons::Ok);
		if (line == "17:0")
			OnRemoteKeyUpPre(0, RemoteButtons::Play);
		if (line == "10:0")
			OnRemoteKeyUpPre(0, RemoteButtons::Menu);
		if (line == "32:0")
			Application::Quit();
	}
*/
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_JOYBUTTONDOWN:
				{
					// printf("- %d\n", event.jbutton.button);
					int which = event.jbutton.which;
					switch (event.jbutton.button) {
						case 2:
							OnRemoteKeyPressPre(which, RemoteButtons::Play);
							break;
						case 3:
							OnRemoteKeyPressPre(which, RemoteButtons::Ok);
							break;
						case 8:
							OnRemoteKeyPressPre(which, RemoteButtons::Menu);
							break;
						case 9:
							OnRemoteKeyPressPre(which, RemoteButtons::Pause);
							break;
						case 0:
							OnRemoteKeyPressPre(which, RemoteButtons::Button3);
							break;
						case 1:
							OnRemoteKeyPressPre(which, RemoteButtons::Button4);
							break;
						case 4:
							OnRemoteKeyPressPre(which, RemoteButtons::Button5);
							break;
						case 5:
							OnRemoteKeyPressPre(which, RemoteButtons::Button6);
							break;
						case 6:
							OnRemoteKeyPressPre(which, RemoteButtons::Button7);
							break;
						case 7:
							OnRemoteKeyPressPre(which, RemoteButtons::Button8);
							break;
						default:
							break;
					}
				}
				break;
			case SDL_JOYBUTTONUP:
				{
					int which = event.jbutton.which;
					switch (event.jbutton.button) {
						case 2:
							OnRemoteKeyUpPre(which, RemoteButtons::Play);
							break;
						case 3:
							OnRemoteKeyUpPre(which, RemoteButtons::Ok);
							break;
						case 8:
							OnRemoteKeyUpPre(which, RemoteButtons::Menu);
							break;
						case 9:
							OnRemoteKeyUpPre(which, RemoteButtons::Pause);
							break;
						case 0:
							OnRemoteKeyUpPre(which, RemoteButtons::Button3);
							break;
						case 1:
							OnRemoteKeyUpPre(which, RemoteButtons::Button4);
							break;
						case 4:
							OnRemoteKeyUpPre(which, RemoteButtons::Button5);
							break;
						case 5:
							OnRemoteKeyUpPre(which, RemoteButtons::Button6);
							break;
						case 6:
							OnRemoteKeyUpPre(which, RemoteButtons::Button7);
							break;
						case 7:
							OnRemoteKeyUpPre(which, RemoteButtons::Button8);
							break;
						default:
							break;
					}
				}
				break;

			case SDL_JOYAXISMOTION:
				{
					int which = event.jaxis.which;
					int buttons;
					switch (which) {
						default:
						case 0:
							buttons = Form::_buttonsPad1;
							break;
						case 1:
							buttons = Form::_buttonsPad2;
							break;
					}

					if (event.jaxis.axis == 0) {
						if (event.jaxis.value < -16000)
							OnRemoteKeyPressPre(which, RemoteButtons::Left);
						else
							if (buttons & RemoteButtons::Left)
								OnRemoteKeyUpPre(which, RemoteButtons::Left);

						if (event.jaxis.value > 16000)
							OnRemoteKeyPressPre(which, RemoteButtons::Right);
						else
							if (buttons & RemoteButtons::Right)
								OnRemoteKeyUpPre(which, RemoteButtons::Right);
					}

					if (event.jaxis.axis == 1) {
						if (event.jaxis.value < -16000)
							OnRemoteKeyPressPre(which, RemoteButtons::Up);
						else
							if (buttons & RemoteButtons::Up)
								OnRemoteKeyUpPre(which, RemoteButtons::Up);

						if (event.jaxis.value > 16000)
							OnRemoteKeyPressPre(which, RemoteButtons::Down);
						else
							if (buttons & RemoteButtons::Down)
								OnRemoteKeyUpPre(which, RemoteButtons::Down);
					}
				}
				break;

			case SDL_KEYDOWN:
				// printf("%d\n", event.key.keysym.sym);
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						if (event.key.keysym.mod & KMOD_LCTRL)
							Application::Quit();
						else
							OnRemoteKeyPressPre(0, RemoteButtons::Menu);
						break;
					case SDLK_RETURN:
						OnRemoteKeyPressPre(0, RemoteButtons::Ok);
						OnKeyPressPre(Keys::Key_ENTER);
						break;
					case SDLK_KP_ENTER:
						OnRemoteKeyPressPre(0, RemoteButtons::Ok);
						OnKeyPressPre(Keys::Key_KP_ENTER);
						break;
					case SDLK_LEFT: OnRemoteKeyPressPre(0, RemoteButtons::Left); break;
					case SDLK_RIGHT: OnRemoteKeyPressPre(0, RemoteButtons::Right); break;
					case SDLK_UP: OnRemoteKeyPressPre(0, RemoteButtons::Up); break;
					case SDLK_DOWN: OnRemoteKeyPressPre(0, RemoteButtons::Down); break;

					case SDLK_QUOTE: OnKeyPressPre(Keys::Key_QUOTE); break;
					case SDLK_COMMA: OnKeyPressPre(Keys::Key_COMMA); break;
					case SDLK_MINUS: OnKeyPressPre(Keys::Key_MINUS); break;
					case SDLK_PERIOD: OnKeyPressPre(Keys::Key_PERIOD); break;
					case SDLK_LALT: OnKeyPressPre(Keys::Key_LALT); break;
					case SDLK_SPACE: OnKeyPressPre(Keys::Key_SPACE); break;
					case SDLK_LSHIFT: OnKeyPressPre(Keys::Key_LSHIFT); break;
					case SDLK_RSHIFT: OnKeyPressPre(Keys::Key_RSHIFT); break;
					case SDLK_BACKSPACE: OnKeyPressPre(Keys::Key_BACKSPACE); break;
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
					case SDLK_f: OnKeyPressPre(Keys::Key_F); break;
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
					case SDLK_w: OnKeyPressPre(Keys::Key_W); break;
					case SDLK_x: OnKeyPressPre(Keys::Key_X); break;
					case SDLK_y: OnKeyPressPre(Keys::Key_Y); break;
					case SDLK_z: OnKeyPressPre(Keys::Key_Z); break;

					case SDLK_F1: OnKeyPressPre(Keys::Key_F1); break;
					case SDLK_F2: OnKeyPressPre(Keys::Key_F2); break;
					case SDLK_F3: OnKeyPressPre(Keys::Key_F3); break;
					case SDLK_F4: OnKeyPressPre(Keys::Key_F4); break;
					case SDLK_F5: OnKeyPressPre(Keys::Key_F5); break;
					case SDLK_F6: OnKeyPressPre(Keys::Key_F6); break;
					case SDLK_F7: OnKeyPressPre(Keys::Key_F7); break;
					case SDLK_F8: OnKeyPressPre(Keys::Key_F8); break;
					case SDLK_F9: OnKeyPressPre(Keys::Key_F9); break;
					case SDLK_F10: OnKeyPressPre(Keys::Key_F10); break;
					case SDLK_F11: OnKeyPressPre(Keys::Key_F11); break;
					case SDLK_F12: OnKeyPressPre(Keys::Key_F12); break;

					case SDLK_KP_0: OnKeyPressPre(Keys::Key_KP0); break;
					case SDLK_KP_1: OnKeyPressPre(Keys::Key_KP1); break;
					case SDLK_KP_2: OnKeyPressPre(Keys::Key_KP2); break;
					case SDLK_KP_3: OnKeyPressPre(Keys::Key_KP3); break;
					case SDLK_KP_4: OnKeyPressPre(Keys::Key_KP4); break;
					case SDLK_KP_5: OnKeyPressPre(Keys::Key_KP5); break;
					case SDLK_KP_6: OnKeyPressPre(Keys::Key_KP6); break;
					case SDLK_KP_7: OnKeyPressPre(Keys::Key_KP7); break;
					case SDLK_KP_8: OnKeyPressPre(Keys::Key_KP8); break;
					case SDLK_KP_9: OnKeyPressPre(Keys::Key_KP9); break;
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
					case SDLK_ESCAPE: OnRemoteKeyUpPre(0, RemoteButtons::Menu); break;
					case SDLK_RETURN:
						OnRemoteKeyUpPre(0, RemoteButtons::Ok);
						OnKeyUpPre(Keys::Key_ENTER);
						break;
					case SDLK_KP_ENTER:
						OnRemoteKeyUpPre(0, RemoteButtons::Ok);
						OnKeyUpPre(Keys::Key_KP_ENTER);
						break;
					case SDLK_LEFT: OnRemoteKeyUpPre(0, RemoteButtons::Left); break;
					case SDLK_RIGHT: OnRemoteKeyUpPre(0, RemoteButtons::Right); break;
					case SDLK_UP: OnRemoteKeyUpPre(0, RemoteButtons::Up); break;
					case SDLK_DOWN: OnRemoteKeyUpPre(0, RemoteButtons::Down); break;

					case SDLK_QUOTE: OnKeyUpPre(Keys::Key_QUOTE); break;
					case SDLK_COMMA: OnKeyUpPre(Keys::Key_COMMA); break;
					case SDLK_MINUS: OnKeyUpPre(Keys::Key_MINUS); break;
					case SDLK_PERIOD: OnKeyUpPre(Keys::Key_PERIOD); break;
					case SDLK_LALT: OnKeyUpPre(Keys::Key_LALT); break;
					case SDLK_SPACE: OnKeyUpPre(Keys::Key_SPACE); break;
					case SDLK_LSHIFT: OnKeyUpPre(Keys::Key_LSHIFT); break;
					case SDLK_RSHIFT: OnKeyUpPre(Keys::Key_RSHIFT); break;
					case SDLK_BACKSPACE: OnKeyUpPre(Keys::Key_BACKSPACE); break;
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

					case SDLK_F1: OnKeyUpPre(Keys::Key_F1); break;
					case SDLK_F2: OnKeyUpPre(Keys::Key_F2); break;
					case SDLK_F3: OnKeyUpPre(Keys::Key_F3); break;
					case SDLK_F4: OnKeyUpPre(Keys::Key_F4); break;
					case SDLK_F5: OnKeyUpPre(Keys::Key_F5); break;
					case SDLK_F6: OnKeyUpPre(Keys::Key_F6); break;
					case SDLK_F7: OnKeyUpPre(Keys::Key_F7); break;
					case SDLK_F8: OnKeyUpPre(Keys::Key_F8); break;
					case SDLK_F9: OnKeyUpPre(Keys::Key_F9); break;
					case SDLK_F10: OnKeyUpPre(Keys::Key_F10); break;
					case SDLK_F11:
						OnKeyUpPre(Keys::Key_F11);
						this->SetFullscreen(!this->GetFullscreen());
						break;
					case SDLK_F12: OnKeyUpPre(Keys::Key_F12); break;

					case SDLK_KP_0: OnKeyUpPre(Keys::Key_KP0); break;
					case SDLK_KP_1: OnKeyUpPre(Keys::Key_KP1); break;
					case SDLK_KP_2: OnKeyUpPre(Keys::Key_KP2); break;
					case SDLK_KP_3: OnKeyUpPre(Keys::Key_KP3); break;
					case SDLK_KP_4: OnKeyUpPre(Keys::Key_KP4); break;
					case SDLK_KP_5: OnKeyUpPre(Keys::Key_KP5); break;
					case SDLK_KP_6: OnKeyUpPre(Keys::Key_KP6); break;
					case SDLK_KP_7: OnKeyUpPre(Keys::Key_KP7); break;
					case SDLK_KP_8: OnKeyUpPre(Keys::Key_KP8); break;
					case SDLK_KP_9: OnKeyUpPre(Keys::Key_KP9); break;
					case SDLK_KP_DIVIDE: OnKeyUpPre(Keys::Key_KP_DIVIDE); break;
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
			case SDL_MOUSEWHEEL: {
				MouseButtons::Enum button = MouseButtons::None;
				if (event.wheel.y < 0)
					button = MouseButtons::XButton2;
				else
					button = MouseButtons::XButton1;
				break;
				if (button) {
						this->mouseButtons &= ~button;
						this->OnMouseUpPre(button, this->mouseButtons);

						this->mouseButtons |= button;
						this->OnMouseDownPre(this->mouseX, this->mouseY, button, this->mouseButtons);
				}
			}

			case SDL_MOUSEBUTTONUP: {
					MouseButtons::Enum button = MouseButtons::None;
					switch (event.button.button) {
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

			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					resizex = event.window.data1;
					resizey = event.window.data2;
				}
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

	int finalWidth, finalHeight;
	Uint32 flags = 0;

	if (this->fullscreen) {
		flags |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP;
		if (this->lastFullscreenState != 1) {
			this->lastWidth = this->GetWidth();
			this->lastHeight = this->GetHeight();
		}

		int windowDisplayIndex = 0;
		if (this->window)
			windowDisplayIndex = SDL_GetWindowDisplayIndex(this->window);

		SDL_DisplayMode current;
		if (SDL_GetDesktopDisplayMode(windowDisplayIndex, &current) == 0) {
			finalWidth = current.w;
			finalHeight = current.h;
		}
	} else {
		flags |= SDL_WINDOW_RESIZABLE;

		if (this->lastFullscreenState <= 0) {
			finalWidth = this->GetWidth();
			finalHeight = this->GetHeight();
		} else {
			finalWidth = this->lastWidth > 0 ? this->lastWidth : this->GetWidth();
			finalHeight = this->lastHeight > 0 ? this->lastHeight : this->GetHeight();
		}
	}

	if (!this->window) {
		// Crear una nueva ventana si aún no existe
		this->window = SDL_CreateWindow(this->text.ToCharArray(),
							SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							finalWidth, finalHeight, flags | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (this->window == NULL) {
			SDL_Log("[ERROR] SDL_CreateWindow failed: %s", SDL_GetError());
			return;
		}
	}
	
	// Actualizar la ventana existente
	this->lastFullscreenState = this->fullscreen ? 1 : 0;
	SDL_SetWindowFullscreen(this->window, this->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	if (!this->context) {
		// Crear un nuevo contexto de renderizado OpenGL si aún no existe
		this->context = SDL_GL_CreateContext(this->window);
		if (this->context == NULL) {
			SDL_Log("[ERROR] SDL_GL_CreateContext failed: %s", SDL_GetError());
			SDL_DestroyWindow(this->window);
			SDL_Quit();
			this->window = 0;
			return;
		}
	}

	if (SDL_GL_MakeCurrent(this->window, this->context) < 0) {
		SDL_Log("[ERROR] SDL_GL_MakeCurrent failed: %s", SDL_GetError());
		return;
	}

	if (SDL_GL_SetSwapInterval(1) < 0) {
		SDL_Log("[ERROR] SDL_GL_SetSwapInterval failed: %s", SDL_GetError());
		return;
	}

	this->SetSize(finalWidth, finalHeight);
}

void Form::SetFullscreen(int mode) {
	if (this->fullscreen == mode)
		return;

	this->fullscreen = mode;

	Bitmap::UnloadAll();
	Drawing::Image::UnloadAll();
	this->RefreshVideo();
}

void Form::SetText(String title) {
	this->text = title;

	if (initialized)
		SDL_SetWindowTitle(this->window, this->text.ToCharArray());
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

bool Form::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	uint32_t * buttons;
	switch (which) {
		default:
		case 0:
			buttons = &Form::_buttonsPad1;
			break;
		case 1:
			buttons = &Form::_buttonsPad2;
			break;
	}

	*buttons |= button;
	// printf("0x%.4X\n", *buttons);
	return Control::OnRemoteKeyPress(which, button);
}

bool Form::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	uint32_t * buttons;
	switch (which) {
		default:
		case 0:
			buttons = &Form::_buttonsPad1;
			break;
		case 1:
			buttons = &Form::_buttonsPad2;
			break;
	}

	*buttons &= ~button;
	// printf("0x%.4X\n", *buttons);
	return Control::OnRemoteKeyUp(which, button);
}

void Form::SwapGL() {
	SDL_GL_SwapWindow(this->window);
}

