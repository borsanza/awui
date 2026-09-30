/**
 * awui/UI/Form.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Form.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/OpenGL/GL.h>
#include <awui/UI/Application.h>
#include <awui/UI/Diagnostics/Stats.h>

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_opengl.h>
#include <algorithm>
#include <cstdlib>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::UI;
using namespace awui::UI::Diagnostics;
using namespace awui::UI::Input;

uint32_t Form::s_buttonsPad1 = 0;
uint32_t Form::s_buttonsPad2 = 0;
std::vector<Form *> *Form::s_formsList = new std::vector<Form *>();

Form::Form() {

	s_formsList->push_back(this);

	m_window = 0;
	m_context = 0;
	m_mouseX = 0;
	m_mouseY = 0;
	m_text = "";
	m_swapInterval = true;

	SetBackColor(Color::FromArgb(192, 192, 192));

	SetBounds(100, 100, 300, 300);
	m_mouseButtons = 0;
	m_mouseControlOver = NULL;
	m_initialized = 0;

	m_lastFullscreenState = -1;
	m_fullscreen = 1;
	m_lastWidth = 0;
	m_lastHeight = 0;

	Stats *stats = Stats::Instance();
	stats->SetDock(DockStyle::None);
	// Es único y compartido: el formulario no lo borra
	AddWidget(stats, WidgetOwnership::Borrowed);
	AddWidget(&m_toast, WidgetOwnership::Borrowed);
}

Form::~Form() {
	s_formsList->erase(std::remove(s_formsList->begin(), s_formsList->end(), this), s_formsList->end());

	if (m_context) {
		SDL_GL_DeleteContext(m_context);
	}

	if (m_window) {
		SDL_DestroyWindow(m_window);
	}
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

	// Habilitar el culling de caras para mejorar el rendimiento, opcional pero recomendado
	glEnable(GL_CULL_FACE);
	//  Especificar qué caras deseas ocultar. Por defecto es GL_BACK, pero aquí la configuramos explícitamente.
	glCullFace(GL_BACK);
	//  Definir cuál es considerada la cara frontal. Por defecto es GL_CCW (counter-clockwise).
	glFrontFace(GL_CCW);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	int r = OnPaintPre(0, 0, GetWidth(), GetHeight(), &gl, true);

	Stats *stats = Stats::Instance();
	stats->SetDrawedControls(r);
}

void Form::OnRemoteHeartbeat() {
	Stats *stats = Stats::Instance();
	stats->OnRemoteHeartbeat();
}

void Form::OnTickPre(float deltaSeconds) {
	Control::OnTickPre(deltaSeconds);
	m_selectionFrame.OnTick(GetChildFocused(), deltaSeconds);
}

void Form::OnTick(float deltaSeconds) {
	// Estadísticas y avisos, por encima de todo (los últimos se pintan encima). Solo se recolocan si alguien ha
	// añadido un control después: moverlos en cada frame recalculaba el foco y la disposición sin necesidad
	Stats *stats = Stats::Instance();
	int count = GetCount();
	if ((IndexOf(stats) != count - 2) || (IndexOf(&m_toast) != count - 1)) {
		MoveToEnd(stats);
		MoveToEnd(&m_toast);
	}

	stats->SetWidth(GetWidth());
	stats->SetLocation(0, GetHeight() - stats->GetHeight());

	int bottom = stats->GetVisible() ? stats->GetTop() : GetHeight();
	m_toast.SetAnchor(40, bottom - 20);
}


void Form::RefreshVideo() {
	if (!m_initialized)
		return;

	// Si no se puede saber el tamaño del escritorio se mantiene el actual; SDL avisará del tamaño real al redimensionar
	int finalWidth = GetWidth();
	int finalHeight = GetHeight();

	if (m_fullscreen) {
		if (m_lastFullscreenState != 1) {
			m_lastWidth = GetWidth();
			m_lastHeight = GetHeight();
		}

		int windowDisplayIndex = 0;
		if (m_window)
			windowDisplayIndex = SDL_GetWindowDisplayIndex(m_window);
		if (windowDisplayIndex < 0)
			windowDisplayIndex = 0;

		SDL_DisplayMode current;
		if (SDL_GetDesktopDisplayMode(windowDisplayIndex, &current) == 0) {
			finalWidth = current.w;
			finalHeight = current.h;
		} else {
			SDL_Log("[ERROR] SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		}
	} else {
		if (m_lastFullscreenState <= 0) {
			finalWidth = GetWidth();
			finalHeight = GetHeight();
		} else {
			finalWidth = m_lastWidth > 0 ? m_lastWidth : GetWidth();
			finalHeight = m_lastHeight > 0 ? m_lastHeight : GetHeight();
		}
	}

	if (!m_window) {
		// La ventana se crea siempre como ventana normal (con bordes y el tamaño del formulario) y luego se pasa a
		// pantalla completa: si se creara ya sin bordes y del tamaño del escritorio, al salir de pantalla completa
		// SDL la devolvería así
		int windowWidth = m_fullscreen ? m_lastWidth : finalWidth;
		int windowHeight = m_fullscreen ? m_lastHeight : finalHeight;
		m_window = SDL_CreateWindow(m_text.ToCharArray(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

	// Sin contexto de OpenGL todavía: se aplicará al crear la ventana (RefreshVideo)
	if (!m_initialized)
		return true;

	if (SDL_GL_SetSwapInterval(m_swapInterval ? 1 : 0) < 0) {
		SDL_Log("[ERROR] SDL_GL_SetSwapInterval failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Form::GetSwapInterval() const {
	return m_swapInterval;
}

void Form::SetFullscreen(int mode) {
	if (m_fullscreen == mode)
		return;

	m_fullscreen = mode;

	// No se descargan las texturas: la ventana y el contexto de OpenGL son los mismos (SDL_SetWindowFullscreen no
	// los recrea, ni en Linux ni en Windows), así que siguen valiendo y no hay tirón al recargarlas todas
	RefreshVideo();
}

void Form::SetText(String title) {
	m_text = title;

	if (m_initialized)
		SDL_SetWindowTitle(m_window, m_text.ToCharArray());
}

bool Form::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	uint32_t *buttons;
	switch (which) {
		default:
		case 0:
			buttons = &Form::s_buttonsPad1;
			break;
		case 1:
			buttons = &Form::s_buttonsPad2;
			break;
	}

	*buttons |= button;
	return Control::OnRemoteKeyPress(which, button);
}

bool Form::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	uint32_t *buttons;
	switch (which) {
		default:
		case 0:
			buttons = &Form::s_buttonsPad1;
			break;
		case 1:
			buttons = &Form::s_buttonsPad2;
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

namespace {
	// Teclas de SDL que entiende awui: la tecla (Keys) y, para las que también manejan los menús, el botón del mando a
	// distancia (RemoteButtons) que simulan. Una sola tabla para pulsar y soltar, así las dos no se desincronizan
	const int NoKey = -1;

	struct KeyMapping {
		SDL_Keycode sdl;
		int key; // Keys::Enum, o NoKey
		RemoteButtons::Enum remote;
	};

	const KeyMapping keyMappings[] = {
		{SDLK_ESCAPE, NoKey, RemoteButtons::Menu},
		{SDLK_RETURN, Keys::Key_ENTER, RemoteButtons::Ok},
		{SDLK_KP_ENTER, Keys::Key_KP_ENTER, RemoteButtons::Ok},
		{SDLK_LEFT, Keys::Key_LEFT, RemoteButtons::Left},
		{SDLK_RIGHT, Keys::Key_RIGHT, RemoteButtons::Right},
		{SDLK_UP, Keys::Key_UP, RemoteButtons::Up},
		{SDLK_DOWN, Keys::Key_DOWN, RemoteButtons::Down},
		{SDLK_QUOTE, Keys::Key_QUOTE, RemoteButtons::None},
		{SDLK_COMMA, Keys::Key_COMMA, RemoteButtons::None},
		{SDLK_MINUS, Keys::Key_MINUS, RemoteButtons::None},
		{SDLK_PERIOD, Keys::Key_PERIOD, RemoteButtons::None},
		{SDLK_LALT, Keys::Key_LALT, RemoteButtons::None},
		{SDLK_RALT, Keys::Key_RALT, RemoteButtons::None},
		{SDLK_LCTRL, Keys::Key_LCTRL, RemoteButtons::None},
		{SDLK_RCTRL, Keys::Key_RCTRL, RemoteButtons::None},
		{SDLK_PLUS, Keys::Key_PLUS, RemoteButtons::None},
		{SDLK_LESS, Keys::Key_LESS, RemoteButtons::None},
		{SDLK_SPACE, Keys::Key_SPACE, RemoteButtons::None},
		{SDLK_LSHIFT, Keys::Key_LSHIFT, RemoteButtons::None},
		{SDLK_RSHIFT, Keys::Key_RSHIFT, RemoteButtons::None},
		{SDLK_BACKSPACE, Keys::Key_BACKSPACE, RemoteButtons::None},
		{SDLK_0, Keys::Key_0, RemoteButtons::None},
		{SDLK_1, Keys::Key_1, RemoteButtons::None},
		{SDLK_2, Keys::Key_2, RemoteButtons::None},
		{SDLK_3, Keys::Key_3, RemoteButtons::None},
		{SDLK_4, Keys::Key_4, RemoteButtons::None},
		{SDLK_5, Keys::Key_5, RemoteButtons::None},
		{SDLK_6, Keys::Key_6, RemoteButtons::None},
		{SDLK_7, Keys::Key_7, RemoteButtons::None},
		{SDLK_8, Keys::Key_8, RemoteButtons::None},
		{SDLK_9, Keys::Key_9, RemoteButtons::None},
		{SDLK_a, Keys::Key_A, RemoteButtons::None},
		{SDLK_b, Keys::Key_B, RemoteButtons::None},
		{SDLK_c, Keys::Key_C, RemoteButtons::None},
		{SDLK_d, Keys::Key_D, RemoteButtons::None},
		{SDLK_e, Keys::Key_E, RemoteButtons::None},
		{SDLK_f, Keys::Key_F, RemoteButtons::None},
		{SDLK_g, Keys::Key_G, RemoteButtons::None},
		{SDLK_h, Keys::Key_H, RemoteButtons::None},
		{SDLK_i, Keys::Key_I, RemoteButtons::None},
		{SDLK_j, Keys::Key_J, RemoteButtons::None},
		{SDLK_k, Keys::Key_K, RemoteButtons::None},
		{SDLK_l, Keys::Key_L, RemoteButtons::None},
		{SDLK_m, Keys::Key_M, RemoteButtons::None},
		{SDLK_n, Keys::Key_N, RemoteButtons::None},
		{SDLK_o, Keys::Key_O, RemoteButtons::None},
		{SDLK_p, Keys::Key_P, RemoteButtons::None},
		{SDLK_q, Keys::Key_Q, RemoteButtons::None},
		{SDLK_r, Keys::Key_R, RemoteButtons::None},
		{SDLK_s, Keys::Key_S, RemoteButtons::None},
		{SDLK_t, Keys::Key_T, RemoteButtons::None},
		{SDLK_u, Keys::Key_U, RemoteButtons::None},
		{SDLK_v, Keys::Key_V, RemoteButtons::None},
		{SDLK_w, Keys::Key_W, RemoteButtons::None},
		{SDLK_x, Keys::Key_X, RemoteButtons::None},
		{SDLK_y, Keys::Key_Y, RemoteButtons::None},
		{SDLK_z, Keys::Key_Z, RemoteButtons::None},
		{SDLK_F1, Keys::Key_F1, RemoteButtons::None},
		{SDLK_F2, Keys::Key_F2, RemoteButtons::None},
		{SDLK_F3, Keys::Key_F3, RemoteButtons::None},
		{SDLK_F4, Keys::Key_F4, RemoteButtons::None},
		{SDLK_F5, Keys::Key_F5, RemoteButtons::None},
		{SDLK_F6, Keys::Key_F6, RemoteButtons::None},
		{SDLK_F7, Keys::Key_F7, RemoteButtons::None},
		{SDLK_F8, Keys::Key_F8, RemoteButtons::None},
		{SDLK_F9, Keys::Key_F9, RemoteButtons::None},
		{SDLK_F10, Keys::Key_F10, RemoteButtons::None},
		{SDLK_F11, Keys::Key_F11, RemoteButtons::None},
		{SDLK_F12, Keys::Key_F12, RemoteButtons::None},
		{SDLK_PAGEUP, Keys::Key_PAGEUP, RemoteButtons::None},
		{SDLK_PAGEDOWN, Keys::Key_PAGEDOWN, RemoteButtons::None},
		{SDLK_HOME, Keys::Key_HOME, RemoteButtons::None},
		{SDLK_END, Keys::Key_END, RemoteButtons::None},
		{SDLK_KP_0, Keys::Key_KP0, RemoteButtons::None},
		{SDLK_KP_1, Keys::Key_KP1, RemoteButtons::None},
		{SDLK_KP_2, Keys::Key_KP2, RemoteButtons::None},
		{SDLK_KP_3, Keys::Key_KP3, RemoteButtons::None},
		{SDLK_KP_4, Keys::Key_KP4, RemoteButtons::None},
		{SDLK_KP_5, Keys::Key_KP5, RemoteButtons::None},
		{SDLK_KP_6, Keys::Key_KP6, RemoteButtons::None},
		{SDLK_KP_7, Keys::Key_KP7, RemoteButtons::None},
		{SDLK_KP_8, Keys::Key_KP8, RemoteButtons::None},
		{SDLK_KP_9, Keys::Key_KP9, RemoteButtons::None},
		{SDLK_KP_DIVIDE, Keys::Key_KP_DIVIDE, RemoteButtons::None},
		{SDLK_KP_EQUALS, Keys::Key_KP_EQUALS, RemoteButtons::None},
		{SDLK_KP_MINUS, Keys::Key_KP_MINUS, RemoteButtons::None},
		{SDLK_KP_MULTIPLY, Keys::Key_KP_MULTIPLY, RemoteButtons::None},
		{SDLK_KP_PERIOD, Keys::Key_KP_PERIOD, RemoteButtons::None},
		{SDLK_KP_PLUS, Keys::Key_KP_PLUS, RemoteButtons::None},
	};

	const KeyMapping *FindKeyMapping(SDL_Keycode code) {
		for (const KeyMapping &mapping : keyMappings) {
			if (mapping.sdl == code)
				return &mapping;
		}

		return nullptr;
	}
} // namespace

void Form::ProcessEvents(SDL_Event *event) {
	int resizex = -1;
	int resizey = -1;

	switch (event->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			const KeyMapping *mapping = FindKeyMapping(event->key.keysym.sym);
			if (!mapping)
				break;

			// Primero el botón del mando a distancia y luego la tecla
			bool pressed = (event->type == SDL_KEYDOWN);
			if (mapping->remote != RemoteButtons::None) {
				if (pressed)
					OnRemoteKeyPressPre(0, mapping->remote);
				else
					OnRemoteKeyUpPre(0, mapping->remote);
			}

			if (mapping->key != NoKey) {
				if (pressed)
					OnKeyPressPre((Keys::Enum) mapping->key);
				else
					OnKeyUpPre((Keys::Enum) mapping->key);
			}

			// F11 al soltarla: pantalla completa
			if (!pressed && (mapping->sdl == SDLK_F11))
				SetFullscreen(!GetFullscreen());
		} break;

		case SDL_MOUSEBUTTONDOWN:
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
				case SDL_BUTTON_X1: // Botones laterales (atrás, adelante)
					button = MouseButtons::XButton1;
					break;
				case SDL_BUTTON_X2:
					button = MouseButtons::XButton2;
					break;
			}

			if (!button)
				break;

			// clicks: 1, o 2 en un doble clic (3 en un triple...)
			if (event->type == SDL_MOUSEBUTTONDOWN) {
				m_mouseButtons |= button;
				OnMouseDownPre(m_mouseX, m_mouseY, button, m_mouseButtons, event->button.clicks);
			} else {
				m_mouseButtons &= ~button;
				OnMouseUpPre(button, m_mouseButtons, event->button.clicks);
			}
			break;
		}

		case SDL_MOUSEWHEEL: {
			int delta = event->wheel.y;
			if (event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				delta = -delta;

			// Si ningún control la usa, cada muesca es como las flechas arriba y abajo: así se recorren los menús.
			// Como mucho 3 pasos por evento (un panel táctil puede mandar muchas muescas de golpe)
			if ((delta != 0) && !OnMouseWheelPre(m_mouseX, m_mouseY, delta)) {
				RemoteButtons::Enum arrow = (delta > 0) ? RemoteButtons::Up : RemoteButtons::Down;
				for (int n = std::min(std::abs(delta), 3); n > 0; n--) {
					OnRemoteKeyPressPre(0, arrow);
					OnRemoteKeyUpPre(0, arrow);
				}
			}
			break;
		}

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
