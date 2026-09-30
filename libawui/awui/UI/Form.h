#pragma once

#include <awui/UI/Control.h>
#include <awui/UI/SelectionFrame.h>
#include <vector>

typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;
typedef union SDL_Event SDL_Event;

namespace awui {
	namespace Diagnostics {
		class Process;
	}

	namespace UI {
		class Form : public Control {
			friend class Application;
			friend class Control;

		  private:
			static std::vector<Form *> *s_formsList;
			static uint32_t s_buttonsPad1;
			static uint32_t s_buttonsPad2;
			Control *m_mouseControlOver;
			// awui::Diagnostics::Process* remoteProcess;
			String m_text;
			SDL_Window *m_window;
			SDL_GLContext m_context;

			int m_mouseX;
			int m_mouseY;
			int m_mouseButtons;
			int m_initialized;

			int m_fullscreen;
			int m_lastFullscreenState;
			int m_lastWidth;
			int m_lastHeight;
			bool m_swapInterval;
			SelectionFrame m_selectionFrame;

			void OnPaintForm();

		  protected:
			// Oculta el de Control (no es virtual): Application y los tests llaman a este en el formulario raíz.
			// El marco de selección avanza cuando todo el árbol ya tiene su posición de este frame
			void OnTickPre(float deltaSeconds);

		  public:
			Form();
			inline SelectionFrame *GetSelectionFrame() { return &m_selectionFrame; }
			virtual ~Form();

			void Init();
			void SetText(String title);
			void RefreshVideo();
			void SetFullscreen(int mode);
			inline int GetFullscreen() const { return m_fullscreen; }

			virtual void OnRemoteHeartbeat();

			virtual void OnTick(float deltaSeconds);
			// Se va a cerrar el programa (tras el último frame, con todo aún en pie)
			virtual void OnClosing() {}

			virtual bool OnRemoteKeyPress(int which, Input::RemoteButtons::Enum button);
			virtual bool OnRemoteKeyUp(int which, Input::RemoteButtons::Enum button);

			inline static uint32_t GetButtonsPad1() { return Form::s_buttonsPad1; }
			inline static uint32_t GetButtonsPad2() { return Form::s_buttonsPad2; }
			void SwapGL();

			uint32_t GetWindowID();
			void ProcessEvents(SDL_Event *event);

			bool SetSwapInterval(bool mode);
			bool GetSwapInterval() const;
		};
	} // namespace UI
} // namespace awui
