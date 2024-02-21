/**
 * awui/Windows/Forms/Form.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Form.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Diagnostics/Process.h>
#include <awui/Drawing/Image.h>
#include <awui/IO/Directory.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Application.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Statistics/Stats.h>

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_opengl.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

Control * Form::m_controlSelected = NULL;
Bitmap * Form::m_selectedBitmap = NULL;
uint32_t Form::m_buttonsPad1 = 0;
uint32_t Form::m_buttonsPad2 = 0;
ArrayList * Form::m_formsList = new ArrayList();

Form::Form() {
	m_formsList->Add(this);
	m_window = 0;
	m_context = 0;
	m_mouseX = 0;
	m_mouseY = 0;
	m_text = "";
	m_swapInterval = true;

	SetBackColor(Drawing::Color::FromArgb(192, 192, 192));

	SetBounds(100, 100, 300, 300);
	m_mouseButtons = 0;
	m_mouseControlOver = NULL;
	m_initialized = 0;

	m_lastFullscreenState = -1;
	m_fullscreen = 1;
	m_lastWidth = 0;
	m_lastHeight = 0;

	Stats * stats = Stats::Instance();
	stats->SetDock(DockStyle::None);
	GetControls()->Add(stats);

//	remoteProcess = new awui::Diagnostics::Process();
//	remoteProcess->Start("cat", "/dev/ttyUSB0 2> /dev/null");
}

Form::~Form() {
	m_formsList->Remove(this);

	if (m_context) {
		SDL_GL_DeleteContext(m_context);
	}

	if (m_window) {
		SDL_DestroyWindow(m_window);
	}

//	delete remoteProcess;
}

bool Form::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Form) {
		return true;
	}

	return Control::IsClass(objectClass);
}

void Form::Init() {
	m_initialized = 1;
	RefreshVideo();
	SetText(m_text);
}

void Form::OnPaintForm() {
	GL gl;
	Drawing::Rectangle rectangle;
	rectangle.SetX(0);
	rectangle.SetY(0);
	rectangle.SetWidth(GetWidth());
	rectangle.SetHeight(GetHeight());

	gl.SetClippingBase(rectangle);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	int r = OnPaintPre(0, 0, GetWidth(), GetHeight(), &gl, true);

	Stats * stats = Stats::Instance();
	stats->SetDrawedControls(r);
}

void Form::OnRemoteHeartbeat() {
	Stats * stats = Stats::Instance();
	stats->OnRemoteHeartbeat();
}

void Form::OnTick(float deltaTime) {
	Stats * stats = Stats::Instance();
	GetControls()->MoveToEnd(stats);
	stats->SetWidth(GetWidth());
	stats->SetLocation(0, GetHeight() - stats->GetHeight());

	static DateTime lastTime = DateTime::GetNow();
	DateTime now = DateTime::GetNow();
	if (lastTime.GetSecond() != now.GetSecond()) {
		lastTime = now;
/*
		if (remoteProcess->GetHasExited()) {
			delete remoteProcess;
			remoteProcess = new awui::Diagnostics::Process();
			remoteProcess->Start("cat", "/dev/ttyUSB0 2> /dev/null");
		}
*/
	}
}

void Form::RefreshVideo() {
	if (!m_initialized)
		return;

	int finalWidth, finalHeight;
	Uint32 flags = 0;

	if (m_fullscreen) {
		flags |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP;
		if (m_lastFullscreenState != 1) {
			m_lastWidth = GetWidth();
			m_lastHeight = GetHeight();
		}

		int windowDisplayIndex = 0;
		if (m_window)
			windowDisplayIndex = SDL_GetWindowDisplayIndex(m_window);

		SDL_DisplayMode current;
		if (SDL_GetDesktopDisplayMode(windowDisplayIndex, &current) == 0) {
			finalWidth = current.w;
			finalHeight = current.h;
		}
	} else {
		flags |= SDL_WINDOW_RESIZABLE;

		if (m_lastFullscreenState <= 0) {
			finalWidth = GetWidth();
			finalHeight = GetHeight();
		} else {
			finalWidth = m_lastWidth > 0 ? m_lastWidth : GetWidth();
			finalHeight = m_lastHeight > 0 ? m_lastHeight : GetHeight();
		}
	}

	if (!m_window) {
		// Crear una nueva ventana si aún no existe
		m_window = SDL_CreateWindow(m_text.ToCharArray(),
							SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							finalWidth, finalHeight, flags | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (m_window == NULL) {
			SDL_Log("[ERROR] SDL_CreateWindow failed: %s", SDL_GetError());
			return;
		}
	}
	
	// Actualizar la ventana existente
	m_lastFullscreenState = m_fullscreen ? 1 : 0;
	SDL_SetWindowFullscreen(m_window, m_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	if (!m_context) {
		// Crear un nuevo contexto de renderizado OpenGL si aún no existe
		m_context = SDL_GL_CreateContext(m_window);
		if (m_context == NULL) {
			SDL_Log("[ERROR] SDL_GL_CreateContext failed: %s", SDL_GetError());
			SDL_DestroyWindow(m_window);
			SDL_Quit();
			m_window = 0;
			return;
		}
	}

	if (SDL_GL_MakeCurrent(m_window, m_context) < 0) {
		SDL_Log("[ERROR] SDL_GL_MakeCurrent failed: %s", SDL_GetError());
		return;
	}

	if (!SetSwapInterval(m_swapInterval)) {
		return;
	}

	SetSize(finalWidth, finalHeight);
}

bool Form::SetSwapInterval(bool mode) {
	m_swapInterval = mode;
	if (SDL_GL_SetSwapInterval(m_swapInterval? 1 : 0) < 0) {
		SDL_Log("[ERROR] SDL_GL_SetSwapInterval failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Form::GetSwapInterval() {
	return m_swapInterval;
}

void Form::SetFullscreen(int mode) {
	if (m_fullscreen == mode)
		return;

	m_fullscreen = mode;

	Bitmap::UnloadAll();
	Drawing::Image::UnloadAll();
	RefreshVideo();
}

void Form::SetText(String title) {
	m_text = title;

	if (m_initialized)
		SDL_SetWindowTitle(m_window, m_text.ToCharArray());
}

Control * Form::GetControlSelected() {
	return Form::m_controlSelected;
}

void Form::SetControlSelected(Control * selected) {
	if (!selected || selected->GetTabStop()) {
		Form::m_controlSelected = selected;
		if (selected) {
			Form::GetSelectedBitmap()->SetParent(selected->GetParent());
			selected->SetFocus(false);
		}
	}
}

Bitmap * Form::GetSelectedBitmap() {
	if (!Form::m_selectedBitmap) {
		String file = IO::Directory::GetWorkingDirectory();
		Bitmap * bitmap = new Bitmap(file + "/images/button.png");
		bitmap->SetDock(DockStyle::None);
		bitmap->SetBackColor(Drawing::Color::FromArgb(0, 0, 0, 0));
		bitmap->SetFixedMargins(28, 25, 28, 24);
		bitmap->SetLocation(0, 0);
		bitmap->SetSize(Drawing::Size(97, 97));
		Form::m_selectedBitmap = bitmap;
	}

	return Form::m_selectedBitmap;
}

bool Form::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	uint32_t * buttons;
	switch (which) {
		default:
		case 0:
			buttons = &Form::m_buttonsPad1;
			break;
		case 1:
			buttons = &Form::m_buttonsPad2;
			break;
	}

	*buttons |= button;
	return Control::OnRemoteKeyPress(which, button);
}

bool Form::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	uint32_t * buttons;
	switch (which) {
		default:
		case 0:
			buttons = &Form::m_buttonsPad1;
			break;
		case 1:
			buttons = &Form::m_buttonsPad2;
			break;
	}

	*buttons &= ~button;
	return Control::OnRemoteKeyUp(which, button);
}

void Form::SwapGL() {
	SDL_GL_SwapWindow(m_window);
}

uint32_t Form::GetWindowID() {
    if (m_window == nullptr) {
		return 0;
	}

    return SDL_GetWindowID(m_window);
}

void Form::ProcessEvents(SDL_Event * event) {
	int resizex = -1;
	int resizey = -1;

	switch(event->type) {
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
				case SDLK_ESCAPE:
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
				case SDLK_LEFT:
					OnRemoteKeyPressPre(0, RemoteButtons::Left);
					OnKeyPressPre(Keys::Key_LEFT);
					break;
				case SDLK_RIGHT:
					OnRemoteKeyPressPre(0, RemoteButtons::Right);
					OnKeyPressPre(Keys::Key_RIGHT);
					break;
				case SDLK_UP:
					OnRemoteKeyPressPre(0, RemoteButtons::Up);
					OnKeyPressPre(Keys::Key_UP);
					break;
				case SDLK_DOWN:
					OnRemoteKeyPressPre(0, RemoteButtons::Down);
					OnKeyPressPre(Keys::Key_DOWN);
					break;
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
			switch (event->key.keysym.sym) {
				case SDLK_ESCAPE: OnRemoteKeyUpPre(0, RemoteButtons::Menu); break;
				case SDLK_RETURN:
					OnRemoteKeyUpPre(0, RemoteButtons::Ok);
					OnKeyUpPre(Keys::Key_ENTER);
					break;
				case SDLK_KP_ENTER:
					OnRemoteKeyUpPre(0, RemoteButtons::Ok);
					OnKeyUpPre(Keys::Key_KP_ENTER);
					break;
				case SDLK_LEFT:
					OnRemoteKeyUpPre(0, RemoteButtons::Left);
					OnKeyUpPre(Keys::Key_LEFT);
					break;
				case SDLK_RIGHT:
					OnRemoteKeyUpPre(0, RemoteButtons::Right);
					OnKeyUpPre(Keys::Key_RIGHT);
					break;
				case SDLK_UP:
					OnRemoteKeyUpPre(0, RemoteButtons::Up);
					OnKeyUpPre(Keys::Key_UP);
					break;
				case SDLK_DOWN:
					OnRemoteKeyUpPre(0, RemoteButtons::Down);
					OnKeyUpPre(Keys::Key_DOWN);
					break;
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
					SetFullscreen(!GetFullscreen());
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

		case SDL_MOUSEBUTTONDOWN: {
				MouseButtons::Enum button = MouseButtons::None;
				switch (event->button.button) {
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
					m_mouseButtons |= button;
					OnMouseDownPre(m_mouseX, m_mouseY, button, m_mouseButtons);
				}
			}
			break;
		case SDL_MOUSEWHEEL: {
			MouseButtons::Enum button = MouseButtons::None;
			if (event->wheel.y < 0)
				button = MouseButtons::XButton2;
			else
				button = MouseButtons::XButton1;
			break;
			if (button) {
					m_mouseButtons &= ~button;
					OnMouseUpPre(button, m_mouseButtons);

					m_mouseButtons |= button;
					OnMouseDownPre(m_mouseX, m_mouseY, button, m_mouseButtons);
			}
		}

		case SDL_MOUSEBUTTONUP: {
				MouseButtons::Enum button = MouseButtons::None;
				switch (event->button.button) {
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
					m_mouseButtons &= ~button;
					OnMouseUpPre(button, m_mouseButtons);
				}
			}
			break;

		case SDL_MOUSEMOTION:
			m_mouseX = event->motion.x;
			m_mouseY = event->motion.y;
			OnMouseMovePre(m_mouseX, m_mouseY, m_mouseButtons);
			break;

		case SDL_WINDOWEVENT:
			if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
				resizex = event->window.data1;
				resizey = event->window.data2;
			}
			break;
	}

	if ((resizex != -1) && (resizey != -1)) {
		SetSize(resizex, resizey);
		RefreshVideo();
	}
}
